/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "gdatabaseiodevice.h"
#include "DataObjects/DataSet/datatable.h"
#include "DataObjects/DataSet/datarow.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>
using namespace GUtil;
using namespace DataObjects;
using namespace DataAccess;

QMutex GDatabaseIODevice::_database_locks_lock;
QStringList GDatabaseIODevice::_occupied_databases;

GDatabaseIODevice::GDatabaseIODevice(const QString &db_connection_id,
                                     QObject *parent)
    :GIODevice(parent),
    _p_IsReady(false),
    _connection_id(db_connection_id),
    _p_WriteCommand(CommandWriteNoop),
    _p_ReadCommand(CommandReadNoop),
    _selection_parameters(0)
{
    _database_locks_lock.lock();
    {
        if(!_occupied_databases.contains(_connection_id))
        {
            _occupied_databases.append(_connection_id);
            _p_IsReady = true;
        }

        if(_p_IsReady)
        {
            // Open the database; we'll close it in our destructor
            QSqlDatabase db(QSqlDatabase::database(_connection_id));

            if(!(_p_IsReady = (db.isOpen() || db.open())))
                // If our open fails, then remove ourselves from the occupied list
                _occupied_databases.removeOne(_connection_id);
        }
    }
    _database_locks_lock.unlock();
}

GDatabaseIODevice::~GDatabaseIODevice()
{
    _database_locks_lock.lock();
    {
        _occupied_databases.removeOne(_connection_id);

        QSqlDatabase::database(_connection_id).close();
        QSqlDatabase::removeDatabase(_connection_id);

        _p_IsReady = false;
    }
    _database_locks_lock.unlock();

    if(_selection_parameters)
        delete _selection_parameters;
}

int GDatabaseIODevice::CreateTable(const QString &name,
                                    const Custom::GPairList<QString, QString> &column_names_n_types,
                                    const QList<int> &key_columns,
                                    bool drop_if_exists)
{
    _fail_if_not_ready();

    int ret(0);

    try
    {
        // First drop the table
        if(drop_if_exists)
            DropTable(name);

        // Then proceed to create the table
        if(_tables.contains(name))
            ret = 1;
        else
        {
            DataTable tbl(name);

            // Prepare column declaration string
            QString cols;
            for(int i = 0; i < column_names_n_types.count(); i++)
            {
                cols.append(QString("%1 %2,")
                            .arg(column_names_n_types[i].first)
                            .arg(column_names_n_types[i].second));

                tbl.AddColumn(column_names_n_types[i].first,
                              column_names_n_types[i].second);
            }

            if(key_columns.count() == 0)
            {
                // Remove the trailing comma
                if(column_names_n_types.count() > 0)
                    cols.remove(cols.length() - 1, 1);
            }
            else
            {
                // Set the primary key columns:
                QString keys;
                for(int i = 0; i < key_columns.count(); i++)
                {
                    tbl.AddKeyColumn(key_columns[i]);
                    keys.append(QString("%1,").arg(tbl.ColumnKeys()[key_columns[i]]));
                }
                keys.remove(keys.length() - 1, 1);

                cols.append(QString("PRIMARY KEY (%1)").arg(keys));
            }

            if(QSqlQuery(QSqlDatabase::database(_connection_id)).exec(
                                        QString("CREATE TABLE IF NOT EXISTS %1 (%2)")
                                        .arg(name)
                                        .arg(cols)))
            {
                _tables.insert(name, tbl);
            }
            else
            {
                ret = 2;
            }
        }
    }
    catch(Core::Exception &)
    {
        ret = 2;
    }

    return ret;
}

bool GDatabaseIODevice::DropTable(const QString &name)
{
    _fail_if_not_ready();

    bool ret(false);

    try
    {
        if(_tables.contains(name))
            _tables.remove(name);

        ret = QSqlQuery(QSqlDatabase(QSqlDatabase::database(_connection_id)))
              .exec(QString("DROP TABLE IF EXISTS %1").arg(name));
    }
    catch(Core::Exception &)
    {
        ret = false;
    }

    return ret;
}

void GDatabaseIODevice::Insert(const DataTable &t)
{
    _fail_if_not_ready();

    if(_tables.contains(t.Name()))
    {
        _p_WriteCommand = CommandInsert;
        SendData(t.ToXmlQString().toAscii());
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(t.Name()).toStdString());
    }
}

DataTable GDatabaseIODevice::Select(const DatabaseSelectionParameters &params)
{
    _fail_if_not_ready();

    DataTable *t(0);
    _p_ReadCommand = CommandSelect;

    if(_tables.contains(params.Table().Name()))
    {
        if(_selection_parameters)
            delete _selection_parameters;

        _selection_parameters = new DatabaseSelectionParameters(params);

        (t = new DataTable())
                ->FromXmlQString(ReceiveData(true));
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(params.Table().Name()).toStdString());
    }

    DataTable ret(*t);
    delete t;
    return ret;
}

long GDatabaseIODevice::Count(const DatabaseSelectionParameters &params)
{
    _fail_if_not_ready();

    DataTable *t(0);
    _p_ReadCommand = CommandCount;

    if(_tables.contains(params.Table().Name()))
    {
        if(_selection_parameters)
            delete _selection_parameters;

        _selection_parameters = new DatabaseSelectionParameters(params);

        (t = new DataTable())
                ->FromXmlQString(ReceiveData(true));
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(params.Table().Name()).toStdString());
    }

    long ret(0);
    Q_ASSERT(t->ColumnCount() == 1);
    Q_ASSERT(t->RowCount() == 1);

    ret = t->Rows()[0][0].value<long>();
    delete t;
    return ret;
}

void GDatabaseIODevice::Update(const DatabaseSelectionParameters &sp,
                               const DatabaseValueParameters &vp)
{
    _fail_if_not_ready();

    if(sp.Table() != vp.Table())
        THROW_NEW_GUTIL_EXCEPTION2(Core::ArgumentException,
                                   "The selection and value parameters originate from different tables");

    if(_tables.contains(sp.Table().Name()))
    {
        _p_WriteCommand = CommandUpdate;
        DataTable t(sp.Table().Clone());

        // First row is selection parameters
        t.ImportRow(sp);

        // Second row is value parameters
        t.ImportRow(vp);

        SendData(t.ToXmlQString().toAscii());
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(sp.Table().Name()).toStdString());
    }
}

void GDatabaseIODevice::Delete(const DatabaseSelectionParameters &p)
{
    _fail_if_not_ready();

    if(_tables.contains(p.Table().Name()))
    {
        _p_WriteCommand = CommandDelete;

        DataTable t(p.Table().Clone());
        t.ImportRow(p);

        SendData(t.ToXmlQString().toAscii());
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(p.Table().Name()).toStdString());
    }
}

DatabaseSelectionParameters GDatabaseIODevice::GetBlankSelectionParameters(
        const QString &table_name)
{
    DatabaseSelectionParameters *p(0);

    if(_tables.contains(table_name))
        p = new DatabaseSelectionParameters(
                _tables[table_name].CreateNewRow()
                );
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(table_name).toStdString());
    }

    DatabaseSelectionParameters ret(*p);
    delete p;
    return ret;
}

DatabaseValueParameters GDatabaseIODevice::GetBlankValueParameters(const QString &table_name)
{
    return GetBlankSelectionParameters(table_name);
}

DataTable GDatabaseIODevice::GetBlankTable(const QString &table_name)
{
    DataTable *t(0);
    if(_tables.contains(table_name))
        t = new DataTable(_tables[table_name].Clone());
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotFoundException,
                                   QString("Table not recognized: (%1)")
                                   .arg(table_name).toStdString());
    }

    DataTable ret(*t);
    delete t;
    return ret;
}

void GDatabaseIODevice::send_data(const QByteArray &d)
        throw(Core::DataTransportException)
{
    _p_ReturnValue.clear();

    DataTable t;
    try
    {
        t.FromXmlQString(d);
    }
    catch(Core::XmlException &ex)
    {
        THROW_NEW_GUTIL_EXCEPTION3(Core::DataTransportException,
                                  "Not a valid XML string",
                                  ex);
    }

    if(t.ColumnCount() == 0 || t.RowCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2(Core::DataTransportException,
                                   "No data to send");

    QSqlDatabase _database = QSqlDatabase::database(_connection_id);
    if(!_database.isOpen() && !_database.open())
        THROW_NEW_GUTIL_EXCEPTION2(Core::DataTransportException,
                                   QString("Unable to open database: %1")
                                   .arg(_database.lastError().text()).toStdString());

    try
    {
        DataRow selection_row(t[0]);
        DataRow values_row(t[ t.RowCount() > 1 ? 1 : 0 ]);

        QSqlQuery query(_database);
        QString where;
        QString values;
        switch(_p_WriteCommand)
        {
        case CommandInsert:
            for(int i = 0; i < t.RowCount(); i++)
            {
                DataRow row(t[i]);

                for(int j = 0; j < t.ColumnCount(); j++)
                {
                    values.append("?,");
                    where.append(QString("%1,").arg(t.ColumnKeys()[j]));
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

                _execute_query(query);

                _p_ReturnValue = query.lastInsertId();
            }
            break;
        case CommandUpdate:
            if(t.KeyColumns().count() == 0)
                THROW_NEW_GUTIL_EXCEPTION2(Core::DataTransportException,
                                           "The table is required to have"
                                           " at least one key column");

            // Prepare the where clause
            for(int j = 0; j < t.ColumnCount(); j++)
            {
                if(!selection_row[j].isNull())
                {
                    where.append(QString("%1=? AND ")
                                .arg(t.ColumnKeys()[j]));
                }
            }

            _execute_query(query);

            break;
        case CommandDelete:
            // Prepare the where clause
            for(int j = 0; j < t.ColumnCount(); j++)
            {
                if(!selection_row[j].isNull())
                {
                    where.append(QString("%1=? AND ")
                                .arg(t.ColumnKeys()[j]));
                }
            }

            if(where.length() == 0)
                THROW_NEW_GUTIL_EXCEPTION2(Core::DataTransportException,
                                           "No parameters set for deletion");

            // Remove the trailing 'and'
            where.remove(where.length() - 5, 5);

            query.prepare(QString("DELETE FROM %1 WHERE %2")
                  .arg(t.Name())
                  .arg(where));

            for(int j = 0; j < t.ColumnCount(); j++)
            {
                if(!selection_row[j].isNull())
                    query.addBindValue(selection_row[j]);
            }

            _execute_query(query);

            break;
        default:
            return;
        }
    }
    catch(Core::Exception &)
    {
        throw;
    }
}

QByteArray GDatabaseIODevice::receive_data()
        throw(Core::DataTransportException)
{
    _p_ReturnValue.clear();

    if(_p_ReadCommand == CommandReadNoop ||
       !_selection_parameters ||
       _selection_parameters->ColumnCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2( Core::DataTransportException,
                                    "You have not specified any parameters "
                                    "for the selection");

    QByteArray ret;
    QSqlDatabase _database = QSqlDatabase::database(_connection_id);

    if(!_database.isOpen() && !_database.open())
        THROW_NEW_GUTIL_EXCEPTION2(Core::DataTransportException,
                                   QString("Unable to open database: %1")
                                   .arg(_database.lastError().text()).toStdString());

    QSqlQuery query(_database);

    try
    {
        QString sql;
        QString values, where;

        // Prepare the where clause; affects all read command types
        for(int i = 0; i < _selection_parameters->Table().ColumnCount(); i++)
        {
            if(!_selection_parameters->At(i).isNull())
            {
                where.append(QString("%1=? AND ")
                             .arg(_selection_parameters->Table().ColumnKeys()[i]));
            }
        }

        if(where.length() > 0)
        {
            where.remove(where.length() - 5, 5);
            where = QString("WHERE %1").arg(where);
        }


        // The following portion depends on the command type
        if(_p_ReadCommand == CommandSelect)
        {
            // Prepare the columns we want to select
            for(int i = 0; i < _selection_parameters->Table().ColumnCount(); i++)
            {
                values.append(QString("%1,")
                              .arg(_selection_parameters->Table().ColumnKeys()[i]));
            }

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
            if(!_selection_parameters->At(i).isNull())
                query.addBindValue(_selection_parameters->At(i));
        }

        if(query.exec())
        {
            DataTable *tbl(0);
            if(_p_ReadCommand == CommandSelect)
            {
                tbl = new DataTable(_selection_parameters->Table().Clone());
                while(query.next())
                {
                    Custom::GVariantList lst;
                    for(int i = 0; i < _selection_parameters->ColumnCount(); i++)
                        lst.append(query.value(i));
                    tbl->AddNewRow(lst);
                }
            }
            else if(_p_ReadCommand == CommandCount)
            {
                if(query.next())
                {
                    tbl = new DataTable(_selection_parameters->Table().Name());
                    tbl->SetColumnHeaders(QStringList("COUNT"));
                    tbl->AddNewRow(Custom::GVariantList() << query.value(0));
                }
            }

            ret = tbl->ToXmlQString().toAscii();
            if(tbl)
                delete tbl;
        }
        else
        {
            Core::DataTransportException ex("Query Failed");
            ex.SetData("db_name", _database.databaseName().toStdString());
            ex.SetData("error", query.lastError().text().toStdString());
            ex.SetData("query", query.executedQuery().toStdString());

            for(int k = 0; k < query.boundValues().count(); k++)
            {
                ex.SetData(QString("Bound Value %1").arg(k).toStdString(),
                           query.boundValue(k).toString().toStdString());
            }

            THROW_GUTIL_EXCEPTION(ex);
        }
    }
    catch(Core::Exception &)
    {
        throw;
    }

    return ret;
}

bool GDatabaseIODevice::has_data_available()
{
    // We can always query the database for more data
    return true;
}

void GDatabaseIODevice::_execute_query(QSqlQuery &query) const
{
    if(!query.exec())
    {
        Core::DataTransportException ex("Query Failed");
        ex.SetData("error", query.lastError().text().toStdString());
        ex.SetData("query", query.executedQuery().toStdString());

        for(int k = 0; k < query.boundValues().count(); k++)
        {
            ex.SetData(QString("Bound Value %1").arg(k).toStdString(),
                       query.boundValue(k).toString().toStdString());
        }

        THROW_GUTIL_EXCEPTION(ex);
    }
}

void GDatabaseIODevice::_fail_if_not_ready() const
{
    if(!GetIsReady())
        THROW_NEW_GUTIL_EXCEPTION2(Core::Exception,
                                   "Database IO device not read for use");
}
