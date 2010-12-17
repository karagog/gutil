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

GDatabaseIODevice::GDatabaseIODevice(const QString &db_connection_id,
                                     QObject *parent)
    :GIODevice(parent),
    _connection_id(db_connection_id),
    _p_WriteCommand(CommandNoop),
    _selection_parameters(0)
{}

GDatabaseIODevice::~GDatabaseIODevice()
{
    if(_selection_parameters)
        delete _selection_parameters;

    QSqlDatabase::database(_connection_id).close();
    QSqlDatabase::removeDatabase(_connection_id);
}

int GDatabaseIODevice::CreateTable(const QString &name,
                                    const Custom::GPairList<QString, QString> &column_names_n_types,
                                    const QList<int> &key_columns,
                                    bool drop_if_exists)
{
    int ret(0);

    try
    {
        // First figure out if we need to drop the table
        if(drop_if_exists && _tables.contains(name))
            DropTable(name);

        if(_tables.contains(name))
            ret = 1;
        else
        {

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
    bool ret(true);

    try
    {
        if(_tables.contains(name))
        {
            _tables.remove(name);
        }
    }
    catch(Core::Exception &)
    {
        ret = false;
    }

    return ret;
}

void GDatabaseIODevice::Insert(const DataTable &t)
{
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
    DataTable *t(0);
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

void GDatabaseIODevice::Update(const DatabaseSelectionParameters &sp,
                               const DatabaseValueParameters &vp)
{

}

void GDatabaseIODevice::Delete(const DatabaseSelectionParameters &p)
{

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
        for(int i = 0; i < t.RowCount(); i++)
        {
            DataRow row(t[i]);

            QSqlQuery query(_database);
            QString where;
            QString values;
            switch(_p_WriteCommand)
            {
            case CommandInsert:
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

                break;
            case CommandUpdate:
                if(t.KeyColumns().count() == 0)
                    THROW_NEW_GUTIL_EXCEPTION2(Core::DataTransportException,
                                               "The table is required to have"
                                               " at least one key column");

                for(int j = 0; j < t.KeyColumns().count(); j++)
                {
                    // The where clause is generated by looking at the key
                    //  columns which have non-null values, and the
                    //  values clause is made by looking at all other columns
                    //  with a value.
                }

                break;
            case CommandDelete:
                for(int j = 0; j < t.ColumnCount(); j++)
                {
                    if(!row[j].isNull())
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
                    if(!row[j].isNull())
                        query.addBindValue(row[j]);
                }

                break;
            default:
                return;
            }

            if(query.exec())
            {
                _p_ReturnValue.clear();

                switch(_p_WriteCommand)
                {
                case CommandInsert:
                    break;
                case CommandUpdate:
                    break;
                case CommandDelete:
                    // This is supposed to return the number of rows affected,
                    //  but I'm not sure if that's what it does
                    _p_ReturnValue = query.value(0);
                    break;
                default:
                    return;
                }
            }
            else
            {
                Core::DataTransportException ex("Query Failed");
                ex.SetData("db_name", _database.databaseName().toStdString());
                ex.SetData("error", query.lastError().text().toStdString());
                ex.SetData("query", query.lastQuery().toStdString());
                ex.SetData("executed", query.executedQuery().toStdString());

                for(int k = 0; k < query.boundValues().count(); k++)
                {
                    ex.SetData(QString("Bound Value %1").arg(k).toStdString(),
                               query.boundValue(k).toString().toStdString());
                }

                THROW_GUTIL_EXCEPTION(ex);
            }
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
    if(!_selection_parameters ||
       _selection_parameters->ColumnCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2( Core::DataTransportException,
                                    "You have not specified any parameters "
                                    "for the selection");

    QByteArray ret;
    QString values, where;
    QSqlDatabase _database = QSqlDatabase::database(_connection_id);

    if(!_database.isOpen() && !_database.open())
        THROW_NEW_GUTIL_EXCEPTION2(Core::DataTransportException,
                                   QString("Unable to open database: %1")
                                   .arg(_database.lastError().text()).toStdString());

    QSqlQuery query(_database);

    try
    {
        for(int i = 0; i < _selection_parameters->Table().ColumnCount(); i++)
        {
            values.append(QString("%1,")
                          .arg(_selection_parameters->Table().ColumnKeys()[i]));

            if(!_selection_parameters->At(i).isNull())
            {
                where.append(QString("%1=? AND ")
                          .arg(_selection_parameters->Table().ColumnKeys()[i]));
            }
        }

        values.remove(values.length() - 1, 1);
        if(where.length() > 0)
        {
            where.remove(where.length() - 5, 5);

            where = QString("WHERE %1").arg(where);
        }


        QString sql(QString("SELECT %1 FROM %2")
                    .arg(values)
                    .arg(_selection_parameters->Table().Name()));

        if(where.length() > 0)
            sql = QString("%1 %2").arg(sql).arg(where);

        query.prepare(sql);

        // Bind values after preparing the query
        for(int i = 0; i < _selection_parameters->ColumnCount(); i++)
        {
            if(!_selection_parameters->At(i).isNull())
                query.addBindValue(_selection_parameters->At(i));
        }

        if(query.exec())
        {
            DataTable tbl(_selection_parameters->Table().Clone());
            while(query.next())
            {
                Custom::GVariantList lst;
                for(int i = 0; i < _selection_parameters->ColumnCount(); i++)
                    lst.append(query.value(i));
                tbl.AddNewRow(lst);
            }

            ret = tbl.ToXmlQString().toAscii();
        }
        else
        {
            Core::DataTransportException ex("Query Failed");
            ex.SetData("db_name", _database.databaseName().toStdString());
            ex.SetData("error", query.lastError().text().toStdString());
            ex.SetData("query", query.lastQuery().toStdString());
            ex.SetData("executed", query.executedQuery().toStdString());

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
