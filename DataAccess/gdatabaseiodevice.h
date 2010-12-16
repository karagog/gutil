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

#ifndef GDATABASEIODEVICE_H
#define GDATABASEIODEVICE_H

#include "giodevice.h"
#include <QSqlDatabase>

namespace GUtil
{
    namespace DataAccess
    {
        // Treat a database like an IO device.  It is a simplified database
        //  representation that does CRUD operations on a single table, with
        //  limited, though very useful in a lot of cases, functionality.  It
        //  is not able to handle foreign relationships or table joins.

        // The data to send/receive is XML data generated from a
        //  DataObjects::DataTable object.  The data you receive from it can be
        //  equivalently read into a DataTable object with the FromXmlQString method

        class GDatabaseIODevice :
                public GIODevice
        {
            Q_OBJECT
        public:

            explicit GDatabaseIODevice(const QSqlDatabase &db = QSqlDatabase(),
                                       QObject *parent = 0);

            inline QSqlDatabase &Database(){ return _database; }
            inline const QSqlDatabase &Database() const{ return _database; }


            // These are the possible read/write commands in enum form
            enum WriteCommandsEnum
            {
                Noop,
                Insert,
                Update,
                Delete
            };

            enum ReadCommandsEnum
            {
                Select
            };


            // Control what commands to issue
            PROPERTY( WriteCommand, WriteCommandsEnum );
            PROPERTY( ReadCommand, ReadCommandsEnum );


        protected:

            void send_data(const QByteArray &)
                    throw(Core::DataTransportException);
            QByteArray receive_data()
                    throw(Core::DataTransportException);

            bool has_data_available();


        private:

            QSqlDatabase _database;

        };

    }
}

#endif // GDATABASEIODEVICE_H
