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
#include "DataObjects/collection.h"
#include "DataObjects/DataSet/datarow.h"
#include <QVariant>
#include <QMap>
#include <QMutex>
#include <QFlags>

class QSqlQuery;

namespace GUtil
{
    namespace Custom
    {
        class GSemaphore;
    }

    namespace DataAccess
    {
        class DatabaseSelectionParameters;
        class DatabaseValueParameters;


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

            // The 'params' variable is a row of data that matches the layout
            //  of the table, and you can set values in it that will be used
            //  to filter the data in the select clause.  It will select the
            //  columns you give it in the 'columns' variable
            DataObjects::DataTable Select(const DatabaseSelectionParameters &where,
                                          const QList<int> &columns = QList<int>());

            // Select all the data in the table
            DataObjects::DataTable Select(const QString &table_name,
                                          const QList<int> &columns = QList<int>());

            // A count query, based on your selection parameter filters
            long Count(const DatabaseSelectionParameters &);

            // A count query that counts all records in the table
            long Count(const QString &table_name);

            // Pass parameters to select out the data you want to update, and
            //  pass values to which to update them
            void Update(const DatabaseSelectionParameters &, const DatabaseValueParameters &);

            void Delete(const DatabaseSelectionParameters &);

            // Call these functions to get a blank table or parameters object that matches the table you declared
            DatabaseSelectionParameters GetBlankSelectionParameters(const QString &table_name);
            DatabaseValueParameters GetBlankValueParameters(const QString &table_name);
            DataObjects::DataTable GetBlankTable(const QString &table_name);

            // The value returned by the last query
            READONLY_PROPERTY( IsReady, bool );
            READONLY_PROPERTY( ReturnValue, QVariant );


            enum ColumnOption
            {
                None,
                CompareNotEquals,
                CompareGreaterThan,
                CompareGreaterThanOrEqualTo,
                CompareLessThan,
                CompareLessThanOrEqualTo,
                Or,
                Not
            };

            Q_DECLARE_FLAGS(ColumnOptions, ColumnOption);


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


            // You can use these low-level functions if you need to generate
            //  byte array commands of your own, but generally you should use
            //  the convenient functions above
            QByteArray prepare_send_data(
                    WriteCommandsEnum cmd,
                    const DataObjects::DataTable &,
                    const DatabaseValueParameters &);

            QByteArray prepare_send_data(
                    WriteCommandsEnum cmd,
                    const DatabaseSelectionParameters &);

            QByteArray prepare_send_data(
                    WriteCommandsEnum cmd,
                    const DatabaseSelectionParameters &,
                    const DatabaseValueParameters &);

            QByteArray prepare_send_data(
                    WriteCommandsEnum cmd,
                    const DataObjects::DataTable &,
                    const DatabaseSelectionParameters &,
                    const DatabaseValueParameters &);


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


            // Control what commands to issue
            ReadCommandsEnum _p_ReadCommand;

            QMap<QString, DataObjects::DataTable> _tables;

            QList<int> _columns_requested;
            DatabaseSelectionParameters *_selection_parameters;

            void _fail_if_not_ready() const;
            void _execute_query(QSqlQuery &) const;

            QString _prepare_where_clause(
                    const QStringList &column_keys,
                    const DatabaseSelectionParameters &) const;

            static QMutex _database_locks_lock;
            static QMap<QString, Custom::GSemaphore *> _occupied_databases;

        };



        class DatabaseParametersBase :
                public Interfaces::IQXmlSerializable
        {
        public:

            // Use ColumnOptions to set various options on the columns, like
            //  a different compare operator, NOT/OR it, etc...
            inline DataObjects::ResizableCollection
                    <GDatabaseIODevice::ColumnOptions> &ColumnOptions(){
                return _column_options;
            }
            inline const DataObjects::ResizableCollection
                    <GDatabaseIODevice::ColumnOptions> &ColumnOptions() const{
                return _column_options;
            }

            inline DataObjects::DataTable Table() const{
                return _row.Table();
            }

            inline int ColumnCount() const{ return _row.ColumnCount(); }

            inline DatabaseParametersBase(
                    const DatabaseParametersBase &p)
                :_column_options(p._column_options),
                _row(p._row)
            {}


        protected:

            inline DatabaseParametersBase(const DataObjects::DataRow &fv)
                :_column_options(fv.ColumnCount()),
                _row(fv)
            {}

            inline DatabaseParametersBase()
                :_row()
            {}

            DataObjects::ResizableCollection<GDatabaseIODevice::ColumnOptions>
                    _column_options;

            // Use the Filtervalues to declare a "where" clause, where every
            //  item in the result set matches the FilterValues.
            DataObjects::DataRow _row;

            void ReadXml(QXmlStreamReader &)
                    throw(Core::XmlException);
            void WriteXml(QXmlStreamWriter &) const;

        };


        class DatabaseSelectionParameters :
                public DatabaseParametersBase
        {
            friend class GDatabaseIODevice;
        public:

            // DataRow is an explicitly shared class, so changes to the
            //   returned object will hold
            inline DataObjects::DataRow FilterValues() const{ return _row; }

            inline DatabaseSelectionParameters(const DatabaseParametersBase &p)
                :DatabaseParametersBase(p)
            {}


        protected:

            inline DatabaseSelectionParameters(const DataObjects::DataRow &r)
                :DatabaseParametersBase(r)
            {}

            inline DatabaseSelectionParameters(){}

        };


        class DatabaseValueParameters :
                public DatabaseParametersBase
        {
            friend class GDatabaseIODevice;
        public:

            // DataRow is an explicitly shared class, so changes to the
            //   returned object will hold
            inline DataObjects::DataRow Values() const{ return _row; }

            inline DatabaseValueParameters(const DatabaseParametersBase &p)
                :DatabaseParametersBase(p)
            {}


        protected:

            inline DatabaseValueParameters(const DataObjects::DataRow &r)
                :DatabaseParametersBase(r)
            {}

            inline DatabaseValueParameters(){}

        };
    }
}

#endif // GDATABASEIODEVICE_H
