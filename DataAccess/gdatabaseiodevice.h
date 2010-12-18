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
#include "Custom/gpairlist.h"
#include "DataObjects/DataSet/datarow.h"
#include <QVariant>
#include <QMap>
#include <QMutex>

class QSqlQuery;

namespace GUtil
{
    namespace Custom
    {
        class GSemaphore;
    }

    namespace DataAccess
    {
        typedef DataObjects::DataRow DatabaseSelectionParameters;
        typedef DataObjects::DataRow DatabaseValueParameters;


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

            explicit GDatabaseIODevice(const QString &db_connection_id,
                                       QObject *parent = 0);
            ~GDatabaseIODevice();

            // The database must be properly configured before using this device
            inline QString GetDatabaseConnectionId(){ return _connection_id; }

            // Here are the various database commands you can issue:

            // You must create a table before using this object for anything
            //  It returns 0 if successful, 1 if the table already exists and 2
            //  if it failed to create the table for some reason
            int CreateTable(const QString &name,
                            const Custom::GPairList<QString, QString> &column_names_n_types,
                            int primary_key_column = -1,
                            bool drop_if_exists = true);

            bool DropTable(const QString &name);


            // Insert the rows in the provided table into the database table of the same name
            //  You can get a blank table to operate on by calling 'GetBlankTable' with the right table name
            void Insert(const DataObjects::DataTable &);

            inline int LastInsertId() const{ return _p_ReturnValue.value<int>(); }

            // The 'params' variable is a row of data that matches the layout of the table, and you can set values in it
            //  that will be used to filter the data in the select clause.
            DataObjects::DataTable Select(const DatabaseSelectionParameters &params);

            // Select all the data in the table
            DataObjects::DataTable Select(const QString &table_name);

            // A count query, based on your selection parameter filters
            long Count(const DatabaseSelectionParameters &);

            // A count query that counts all records in the table
            inline long Count(const QString &table_name){ return Count(GetBlankSelectionParameters(table_name)); }


            // Pass parameters to select out the data you want to update, and
            //  pass values to which to update them
            void Update(const DatabaseSelectionParameters &, const DatabaseValueParameters &);

            void Delete(const DatabaseSelectionParameters &);

            // Call these functions to get a blank table or parameters object that matches the table you declared
            DatabaseSelectionParameters GetBlankSelectionParameters(const QString &table_name);
            DatabaseValueParameters GetBlankValueParameters(const QString &table_name);
            DataObjects::DataTable GetBlankTable(const QString &table_name);

            // This is true if the io device is ready to use (must supply a valid database
            READONLY_PROPERTY( IsReady, bool );


            READONLY_PROPERTY( ReturnValue, QVariant );


        private:

            // Here are the overridden methods from GIODevice
            //  Hide these members from the public; nobody should use this interface.
            inline void SendData(const QByteArray &d)
                    throw(Core::DataTransportException){ GIODevice::SendData(d); }
            inline QByteArray ReceiveData(bool block)
                    throw(Core::DataTransportException){ return GIODevice::ReceiveData(block); }

            void send_data(const QByteArray &)
                    throw(Core::DataTransportException);
            QByteArray receive_data()
                    throw(Core::DataTransportException);

            bool has_data_available();


            // A reference to the QSqlDatabase connection
            QString _connection_id;

            // These are the possible read/write commands in enum form
            enum WriteCommandsEnum
            {
                CommandWriteNoop,
                CommandInsert,
                CommandUpdate,
                CommandDelete
            };

            enum ReadCommandsEnum
            {
                CommandReadNoop,
                CommandSelect,
                CommandCount
            };


            // Control what commands to issue
            WriteCommandsEnum _p_WriteCommand;
            ReadCommandsEnum _p_ReadCommand;

            QMap<QString, DataObjects::DataTable> _tables;
            DatabaseSelectionParameters *_selection_parameters;

            void _fail_if_not_ready() const;
            void _execute_query(QSqlQuery &) const;

            static QMutex _database_locks_lock;
            static QMap<QString, Custom::GSemaphore *> _occupied_databases;

        };

    }
}

#endif // GDATABASEIODEVICE_H
