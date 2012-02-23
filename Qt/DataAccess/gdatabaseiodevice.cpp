/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifdef DATABASE_FUNCTIONALITY

#include "Core/extendedexception.h"
#include "gdatabaseiodevice.h"
#include "Utils/databaseutils.h"
#include "Core/Utils/smartpointer.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>
#include <QSqlDatabase>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL1(Utils);
USING_NAMESPACE_GUTIL2(QT, DataObjects);
USING_NAMESPACE_GUTIL2(QT, Utils);

NAMESPACE_GUTIL2(QT, DataAccess);


GDatabaseIODevice::GDatabaseIODevice(const QString &db_connection_id,
                                     QObject *parent)
    :GIODevice(parent),
    _connection_id(db_connection_id),
    _p_ReadCommand(CommandReadNoop),
    _selection_parameters(0)
{
    QSqlDatabase db(QSqlDatabase::database(_connection_id));
    QString driver(db.driverName());

    if(IsReady())
    {
        // Reverse engineer the database schema
        if(driver == "QSQLITE")
        {
            QSqlQuery q("SELECT tbl_name FROM sqlite_master WHERE type = 'table'",
                        db);
            if(!q.exec())
                DatabaseUtils::ThrowQueryException(q);

            while(q.next())
            {
                QString table_name( q.value(0).toString() );

                // Get the table's column names
                Vector<String> columns;
                QSqlQuery c(QString("PRAGMA table_info(%1)").arg(table_name),
                            db);
                if(!c.exec())
                    DatabaseUtils::ThrowQueryException(q);

                while(c.next())
                    columns.PushBack(String::FromQString(c.value(1).toString()));

                _tables.insert(table_name, DataObjects::VariantTable(columns));
            }
        }
        //    else if(driver == "QMYSQL")
        //    {

        //    }
        else
        {
            THROW_NEW_GUTIL_EXCEPTION2(NotImplementedException,
                                       QString("Database driver '%1' not implemented")
                                       .arg(driver).toUtf8());
        }
    }
}

GDatabaseIODevice::~GDatabaseIODevice()
{
    if(_selection_parameters)
        delete _selection_parameters;
}

bool GDatabaseIODevice::IsReady() const
{
    return QSqlDatabase::database(_connection_id).isOpen();
}

void GDatabaseIODevice::CreateTable(const QString &name,
                                    const PairList<QString, QString> &column_names_n_types,
                                    int primary_key_column,
                                    bool drop_if_exists)
{
    _fail_if_not_ready();

    // First drop the table
    if(drop_if_exists)
        DropTable(name);

    // Then proceed to create the table
    if(!_tables.contains(name))
    {
        VariantTable tbl(String::FromQString(name));

        // Prepare column declaration string
        QSqlDatabase db(QSqlDatabase::database(_connection_id));
        QSqlQuery q(db);
        QString driver(db.driverName());

        if(driver == "QSQLITE")
        {
            QString cols;
            for(GUINT32 i = 0; i < column_names_n_types.Count(); i++)
            {
                cols.append(QString("%1 %2,")
                            .arg(column_names_n_types[i].First)
                            .arg(column_names_n_types[i].Second));

                tbl.Columns().Add(DataColumn(column_names_n_types[i].First,
                                             column_names_n_types[i].Second));
            }

            if(primary_key_column == -1)
            {
                // Remove the trailing comma
                if(column_names_n_types.Count() > 0)
                    cols.remove(cols.length() - 1, 1);
            }
            else
            {
                // Set the primary key column:
                cols.append(QString("PRIMARY KEY (%1)")
                            .arg(tbl.Columns()[primary_key_column].Value().GetKey()));
            }

            q.prepare(QString("CREATE TABLE IF NOT EXISTS %1 (%2);")
                      .arg(name)
                      .arg(cols));
        }

        if(!q.exec())
            DatabaseUtils::ThrowQueryException(q);
        _tables.insert(name, tbl);
    }
}

bool GDatabaseIODevice::DropTable(const QString &name)
{
    _fail_if_not_ready();

    bool ret(false);

    QSqlDatabase db(QSqlDatabase::database(_connection_id));
    QSqlQuery q(db);
    QString driver(db.driverName());

    if(driver == "QSQLITE")
    {
        ret = q.exec(QString("DROP TABLE IF EXISTS %1;").arg(name));
    }

    if(!ret)
        DatabaseUtils::ThrowQueryException(q);

    if(_tables.contains(name))
        _tables.remove(name);

    return ret;
}

QString GDatabaseIODevice::CreateIndex(const QString &table_name,
                                       const QStringList &columns)
{
    _fail_if_not_ready();

    QSqlDatabase db(QSqlDatabase::database(_connection_id));
    QSqlQuery q(db);
    QString driver(db.driverName());

    QString index_name(QString("idx_%1_%2").arg(table_name).arg(columns.join("_")));

    bool res(false);
    if(driver == "QSQLITE")
    {
        res = q.exec(QString("CREATE INDEX IF NOT EXISTS %1 ON %2(%3);")
                     .arg(index_name)
                     .arg(table_name)
                     .arg(columns.join(", ")));
    }

    if(!res)
        DatabaseUtils::ThrowQueryException(q);

    return index_name;
}

void GDatabaseIODevice::DropIndex(const QString &index_name)
{
    _fail_if_not_ready();

    QSqlDatabase db(QSqlDatabase::database(_connection_id));
    QSqlQuery q(db);
    QString driver(db.driverName());

    if(driver == "QSQLITE")
    {
        if(!q.exec(
                QString("DROP INDEX IF EXISTS %1;")
                .arg(index_name)))
        {
            DatabaseUtils::ThrowQueryException(q);
        }
    }
}

void GDatabaseIODevice::Insert(const VariantTable &t)
{
    _fail_if_not_ready();

    if(_tables.contains(t.Name()))
    {
        SendData( prepare_send_data(CommandInsert, t) );
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(t.Name()).toUtf8());
    }
}

VariantTable GDatabaseIODevice::Select(
        const DatabaseSelectionParameters &where,
        const QStringList &columns)
{
    QList<int> cols;
    {
        VariantTable t(where.Table());
        foreach(QString s, columns)
            cols.append(t.ColumnIndex(String::FromQString(s)));
    }

    return Select(where, cols);
}

VariantTable GDatabaseIODevice::Select(const QString &table_name,
                                       const QStringList &columns)
{
    QList<int> cols;
    {
        VariantTable t(_tables[table_name]);
        foreach(QString s, columns)
            cols.append(t.ColumnIndex(s));
    }

    return Select(table_name, cols);
}

VariantTable GDatabaseIODevice::Select(const QString &table_name,
                                       const QList<int> &cols)
{
    return Select(GetBlankSelectionParameters(table_name), cols);
}

VariantTable GDatabaseIODevice::Select(const DatabaseSelectionParameters &params,
                                       const QList<int> &columns)
{
    _fail_if_not_ready();

    VariantTable *t(0);
    _p_ReadCommand = CommandSelect;

    if(_tables.contains(params.Table().Name()))
    {
        if(_selection_parameters)
        {
            delete _selection_parameters;
            _columns_requested.clear();
        }

        _selection_parameters = new DatabaseSelectionParameters(params);
        _columns_requested = columns;

        (t = new VariantTable)->FromXmlQString(ReceiveData(true));
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(params.Table().Name()).toUtf8());
    }

    VariantTable ret(*t);
    delete t;
    return ret;
}

long GDatabaseIODevice::Count(const DatabaseSelectionParameters &params)
{
    _fail_if_not_ready();

    VariantTable *t(0);
    _p_ReadCommand = CommandCount;

    if(_tables.contains(params.Table().Name()))
    {
        if(_selection_parameters)
            delete _selection_parameters;

        _selection_parameters = new DatabaseSelectionParameters(params);

        (t = new VariantTable)->FromXmlQString(ReceiveData(true));
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(params.Table().Name()).toUtf8());
    }

    long ret(0);
    GASSERT(t->ColumnCount() == 1);
    GASSERT(t->RowCount() == 1);

    ret = t->operator [](0)[0]->value<long>();
    delete t;
    return ret;
}

long GDatabaseIODevice::Count(const QString &table_name)
{
    return Count(GetBlankSelectionParameters(table_name));
}

void GDatabaseIODevice::Update(const DatabaseSelectionParameters &sp,
                               const DatabaseValueParameters &vp)
{
    _fail_if_not_ready();

    if(_tables.contains(sp.Table().Name()))
    {
        SendData( prepare_send_data(CommandUpdate, sp, vp) );
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(sp.Table().Name()).toUtf8());
    }
}

void GDatabaseIODevice::Delete(const DatabaseSelectionParameters &p)
{
    _fail_if_not_ready();

    if(_tables.contains(p.Table().Name()))
    {
        SendData( prepare_send_data(CommandDelete, p) );
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(p.Table().Name()).toUtf8());
    }
}

DatabaseSelectionParameters GDatabaseIODevice::GetBlankSelectionParameters(
        const QString &table_name) const
{
    DatabaseSelectionParameters *p(0);

    if(_tables.contains(table_name))
        p = new DatabaseSelectionParameters(
                *_tables[table_name].CreateRow()
                );
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(table_name).toUtf8());
    }

    DatabaseSelectionParameters ret(*p);
    delete p;
    return ret;
}

DatabaseValueParameters GDatabaseIODevice::GetBlankValueParameters(const QString &table_name) const
{
    DatabaseValueParameters *p(0);

    if(_tables.contains(table_name))
        p = new DatabaseValueParameters(
                *_tables[table_name].CreateRow()
                );
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(table_name).toUtf8());
    }

    DatabaseValueParameters ret(*p);
    delete p;
    return ret;
}

VariantTable GDatabaseIODevice::GetBlankTable(const QString &table_name) const
{
    VariantTable *t(0);
    if(_tables.contains(table_name))
        t = new VariantTable(_tables[table_name]);
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(table_name).toUtf8());
    }

    VariantTable ret(*t);
    delete t;
    return ret;
}

void GDatabaseIODevice::send_data(const QByteArray &d)
{
    _p_ReturnValue.clear();

    WriteCommandsEnum write_cmd(CommandWriteNoop);
    DatabaseSelectionParameters sp;
    DatabaseValueParameters vp;
    VariantTable t;
    try
    {
        bool ok(false);
        QXmlStreamReader sr(d);
        if(!sr.readNextStartElement() || sr.name() != "x")
            THROW_NEW_GUTIL_EXCEPTION(XmlException);

        write_cmd = (WriteCommandsEnum)
                sr.attributes().at(0).value().toString().toInt(&ok);
        GASSERT(ok);

        sp.ReadXml(sr);
        vp.ReadXml(sr);
        t.ReadXml(sr);
    }
    catch(const XmlException<> &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION3(DataTransportException,
                                   "Not a valid XML string",
                                   ex);
    }

    if(t.ColumnCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException,
                                   "No data to send");

    QSqlDatabase _database = QSqlDatabase::database(_connection_id);
    if(!IsReady())
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException,
                                   "Database not ready");


    QSqlQuery query(_database);
    QString where;
    QString values;

    QString oper("=");
    QString conjunction("AND");
    switch(write_cmd)
    {
    case CommandInsert:

        for(GUINT32 i = 0; i < t.RowCount(); i++)
        {
            DataRow<QVariant> &row(t[i]);

            values.clear(); where.clear();
            for(int j = 0; j < t.ColumnCount(); j++)
            {
                values.append("?,");
                where.append(QString("%1,").arg(t.Columns()[j]->GetKey()));
            }

            // Remove the trailing comma
            values.remove(values.length() - 1, 1);
            where.remove(where.length() - 1, 1);

            // Prepare the query string
            query.prepare(QString("INSERT INTO %1 (%2) VALUES (%3)")
                          .arg(t.Name())
                          .arg(where)
                          .arg(values));

            // Bind values after preparing the query
            for(int j = 0; j < t.ColumnCount(); j++)
                query.addBindValue(row[j]);

            if(!query.exec())
                DatabaseUtils::ThrowQueryException(query);

            _p_ReturnValue = query.lastInsertId();
        }

        break;


    case CommandUpdate:

        where = _prepare_where_clause(t.ColumnKeys(), sp);

        // Prepare the values clause
        for(int j = 0; j < vp.ColumnCount(); j++)
        {
            if(!vp.Values()[j].Value().isNull())
            {
                values.append(QString("%1 = ?,")
                              .arg(t.Columns()[j]->GetKey()));
            }
        }
        if(values.length() > 0)
            values.remove(values.length() - 1, 1);

        // Prepare the query
        query.prepare(QString("UPDATE %1 SET %2 %3")
                      .arg(t.Name())
                      .arg(values)
                      .arg(where));

        // Bind the parameters
        for(int j = 0; j < vp.ColumnCount(); j++)
        {
            if(!vp.Values()[j].Value().isNull())
                query.addBindValue(vp.Values()[j]);
        }
        for(int j = 0; j < sp.ColumnCount(); j++)
        {
            if(!sp.FilterValues()[j].Value().isNull())
                query.addBindValue(sp.FilterValues()[j]);
        }

        // Execute
        if(!query.exec())
            DatabaseUtils::ThrowQueryException(query);

        break;


    case CommandDelete:

        where = _prepare_where_clause(t.ColumnKeys(), sp);

        query.prepare(QString("DELETE FROM %1 %2")
                      .arg(t.Name())
                      .arg(where));

        for(int j = 0; j < t.ColumnCount(); j++)
        {
            if(!sp.FilterValues()[j].Value().isNull())
                query.addBindValue(sp.FilterValues()[j]);
        }

        if(!query.exec())
            DatabaseUtils::ThrowQueryException(query);

        break;


    default:
        return;
    }
}

QString GDatabaseIODevice::_prepare_where_clause(const Vector<String> &column_keys,
                                                 const DatabaseSelectionParameters &sp) const
{
    QString where;
    QString oper("=");
    QString conjunction("AND");

    for(int j = 0; j < sp.ColumnCount(); j++)
    {
        if(!sp.FilterValues()[j]->isNull())
        {
            QFlags<ColumnOption> opts = sp.ColumnOptions()[j];
            QString n(opts.testFlag(Not) ? "NOT" : "");

            if(opts.testFlag(CompareGreaterThan))
                oper = ">";
            else if(opts.testFlag(CompareLessThan))
                oper = "<";
            else if(opts.testFlag(CompareGreaterThanOrEqualTo))
                oper = ">=";
            else if(opts.testFlag(CompareLessThanOrEqualTo))
                oper = "<=";
            else if(opts.testFlag(CompareNotEquals))
                oper = "<>";

//                    if(sp.ColumnOptions()[j].testFlag(Or))
//                        conjunction = "OR";

            where.append(QString("%1 %2 %3 ? %4 ")
                         .arg(n)
                        .arg(column_keys[j].ToQString())
                        .arg(oper)
                        .arg(conjunction));
        }
    }
    if(where.length() > 0)
    {
        where.remove(where.length() - (conjunction.length() + 2),
                     conjunction.length() + 2);
        where = QString("WHERE %1").arg(where);
    }

    return where;
}

QByteArray GDatabaseIODevice::receive_data()
{
    _p_ReturnValue.clear();

    if(_p_ReadCommand == CommandReadNoop ||
       !_selection_parameters ||
       _selection_parameters->ColumnCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2( DataTransportException,
                                    "You have not specified any parameters "
                                    "for the selection");

    QByteArray ret;
    QSqlDatabase _database = QSqlDatabase::database(_connection_id);

    if(!IsReady())
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException,
                                   "Database not ready");

    QSqlQuery query(_database);

    QString sql;
    QString values, where;

    // Prepare the where clause; affects all read command types
    where = _prepare_where_clause(_selection_parameters->Table().ColumnKeys(),
                                  *_selection_parameters);


    // The following portion depends on the command type
    if(_p_ReadCommand == CommandSelect)
    {
        // Prepare the columns we want to select
        for(int i = 0; i < _selection_parameters->Table().ColumnCount(); i++)
        {
            if(_columns_requested.isEmpty() ||
                    _columns_requested.contains(i))
            {
                values.append(QString("%1,").arg(
                                  _selection_parameters->Table().Columns()[i].GetKey()));
            }
        }
        GASSERT(values.length() > 0);

        values.remove(values.length() - 1, 1);

        sql = QString("SELECT %1 FROM %2 %3")
                .arg(values)
                .arg(_selection_parameters->Table().Name())
                .arg(where);
    }
    else if(_p_ReadCommand == CommandCount)
    {
        sql = QString("SELECT COUNT(*) FROM %1 %2")
                .arg(_selection_parameters->Table().Name())
                .arg(where);
    }



    // Prepare and Execute the query (all command types)
    query.prepare(sql);

    for(int i = 0; i < _selection_parameters->ColumnCount(); i++)
    {
        if(!_selection_parameters->FilterValues()[i]->isNull())
            query.addBindValue(_selection_parameters->FilterValues()[i]);
    }

    if(query.exec())
    {
        VariantTable *tbl(0);
        if(_p_ReadCommand == CommandSelect)
        {
            tbl = new VariantTable(_selection_parameters->Table());
            while(query.next())
            {
                Vector<QVariant> lst;
                if(_columns_requested.isEmpty())
                {
                    for(int i = 0; i < _selection_parameters->ColumnCount(); i++)
                        lst.PushBack(query.value(i));
                }
                else
                {
                    lst.Resize(_selection_parameters->ColumnCount());

                    for(int i = 0; i < _columns_requested.count(); i++)
                        lst[_columns_requested[i]] = query.value(i);
                }
                tbl->AddRow(lst);
            }
        }
        else if(_p_ReadCommand == CommandCount)
        {
            if(query.next())
            {
                tbl = new VariantTable(_selection_parameters->Table().Name());
                tbl->AddColumn("COUNT");
                tbl->AddRow(Vector<QVariant>() << query.value(0));
            }
        }

        ret = tbl->ToXmlQString().toAscii();
        delete tbl;
    }
    else
    {
        DatabaseUtils::ThrowQueryException(query);
    }

    return ret;
}

bool GDatabaseIODevice::has_data_available()
{
    // We can always query the database for more data
    return true;
}

QByteArray GDatabaseIODevice::prepare_send_data(WriteCommandsEnum cmd,
                                              const VariantTable &t)
{
    QString tbl(t.Name().ToQString());
    return prepare_send_data(cmd, t,
                             GetBlankSelectionParameters(tbl),
                             GetBlankValueParameters(tbl));
}

QByteArray GDatabaseIODevice::prepare_send_data(WriteCommandsEnum cmd,
                                                const DatabaseSelectionParameters &sp)
{
    QString tbl(sp.Table().Name().ToQString());
    return prepare_send_data(cmd, GetBlankTable(tbl), sp,
                             GetBlankValueParameters(tbl));
}

QByteArray GDatabaseIODevice::prepare_send_data(WriteCommandsEnum cmd,
                                                const DatabaseSelectionParameters &sp,
                                                const DatabaseValueParameters &vp)
{
    return prepare_send_data(cmd, GetBlankTable(sp.Table().Name()), sp, vp);
}

QByteArray GDatabaseIODevice::prepare_send_data(WriteCommandsEnum cmd,
                                                const VariantTable &t,
                                                const DatabaseSelectionParameters &sp,
                                                const DatabaseValueParameters &vp)
{
    QByteArray ret;
    QXmlStreamWriter sw(&ret);

    sw.writeStartElement("x");
    sw.writeAttribute("c", QString("%1").arg((int)cmd));
    {
        sp.WriteXml(sw);
        vp.WriteXml(sw);
        t.WriteXml(sw);
    }
    sw.writeEndElement();
    return ret;
}

void GDatabaseIODevice::OpenDatabaseConnection()
{
    QSqlDatabase db(QSqlDatabase::database(GetDatabaseConnectionId()));
    if(!db.isValid())
        THROW_NEW_GUTIL_EXCEPTION2(Exception, String::Format("Invalid database: %s", GetDatabaseConnectionId().toUtf8().constData()));
    if(!db.isOpen()){
        if(!db.open())
            THROW_NEW_GUTIL_EXCEPTION2(Exception, db.lastError().text().toUtf8());
    }
}

void GDatabaseIODevice::CloseDatabaseConnection()
{
    QSqlDatabase db(QSqlDatabase::database(GetDatabaseConnectionId()));
    if(db.isValid())
        db.close();
}





void DatabaseParametersBase::ReadXml(QXmlStreamReader &sr)
{
    SharedSmartPointer< DataRow<QVariant> > col_opts(Table().CreateRow());

    VariantTable::ReadRowXml(sr, _row);
    VariantTable::ReadRowXml(sr, *col_opts);

    ColumnOptions().Clear();
    for(int i = 0; i < col_opts->ColumnCount(); i++)
        ColumnOptions().Add((GDatabaseIODevice::ColumnOption)(*col_opts)[i]->toInt());
}

void DatabaseParametersBase::WriteXml(QXmlStreamWriter &sw) const
{
    Vector<QVariant> gvl;
    for(int i = 0; i < ColumnOptions().Count(); i++)
        gvl.PushBack((int)ColumnOptions()[i]);

    SharedSmartPointer< DataRow<QVariant> > col_opts(Table().CreateRow(gvl));

    VariantTable::WriteRowXml(sw, _row);
    VariantTable::WriteRowXml(sw, *col_opts);
}


END_NAMESPACE_GUTIL2;

#endif // DATABASE_FUNCTIONALITY
