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
using namespace GUtil;
using namespace DataObjects;
using namespace DataAccess;

GDatabaseIODevice::GDatabaseIODevice(const QSqlDatabase &db,
                                     QObject *parent)
    :GIODevice(parent),
    _p_WriteCommand(Noop),
    _p_ReadCommand(Select),
    _database(db)
{

}

void GDatabaseIODevice::send_data(const QByteArray &d)
        throw(Core::DataTransportException)
{
    DataTable t;

    try
    {

    }
    catch(Core::XmlException &)
    {
        THROW_NEW_GUTIL_EXCEPTION2(Core::DataTransportException,
                                  "Not a valid XML string");
    }

    switch(GetWriteCommand())
    {
    case Insert:

        break;
    case Update:
        break;
    case Delete:
        break;
    default:
        return;
    }
}

QByteArray GDatabaseIODevice::receive_data()
        throw(Core::DataTransportException)
{
    QByteArray ret;

    return ret;
}

bool GDatabaseIODevice::has_data_available()
{
    // We can always query the database for more data
    return true;
}
