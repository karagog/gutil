/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_NO_DATABASE_FUNCTIONALITY


#ifndef GDATABASEIODEVICE_H
#define GDATABASEIODEVICE_H

#include "gutil_iodevice.h"
#include "gutil_pairlist.h"
#include "gutil_collection.h"
#include "gutil_varianttable.h"
#include <QVariant>
#include <QMap>
#include <QMutex>
#include <QFlags>

class QSqlQuery;
class QSqlDatabase;

namespace GUtil{ namespace QT{
namespace DataObjects{
    class VariantTable;
}
namespace DataAccess{


class DatabaseSelectionParameters;
class DatabaseValueParameters;


// Treat a database like an IO device.  It is a simplified database
//  representation that does CRUD operations on a single table, with
//  limited, though very useful in a lot of cases, functionality.  It
//  is not able to handle foreign relationships or table joins.

// The data to send/receive is XML data generated from a
//  DataObjects::VariantTable object.  The data you receive from it can be
//  equivalently read into a VariantTable object with the FromXmlQString method

class DatabaseIODevice :
        public IODevice
{
    Q_OBJECT
public:

    explicit DatabaseIODevice(const QString &db_connection_id,
                               QObject *parent = 0);
    ~DatabaseIODevice();

    // The database must be properly configured before using this device
    inline QString GetDatabaseConnectionId(){ return _connection_id; }

    void OpenDatabaseConnection();
    void CloseDatabaseConnection();

    // Here are the various database commands you can issue:

    void CreateTable(const QString &name,
                     const GUtil::DataObjects::PairList<QString, QString> &column_names_n_types,
                     int primary_key_column = -1,
                     bool drop_if_exists = true);

    bool DropTable(const QString &name);

    inline QStringList GetTableList() const{ return _tables.keys(); }

    // Returns the index name of the created index (throws an exception if error)
    QString CreateIndex(const QString &table_name,
                        const QStringList &columns);
    void DropIndex(const QString &index_name);


    // Insert the rows in the provided table into the database table of the same name
    //  You can get a blank table to operate on by calling 'GetBlankTable' with the right table name
    void Insert(const DataObjects::VariantTable &);

    inline int LastInsertId() const{ return _p_ReturnValue.value<int>(); }

    // The 'params' variable is a row of data that matches the layout
    //  of the table, and you can set values in it that will be used
    //  to filter the data in the select clause.  It will select the
    //  columns you give it in the 'columns' variable
    DataObjects::VariantTable Select(const DatabaseSelectionParameters &where,
                                  const QList<int> &columns = QList<int>());

    // Select all the data in the table
    DataObjects::VariantTable Select(const QString &table_name,
                                  const QList<int> &columns = QList<int>());

    DataObjects::VariantTable Select(const DatabaseSelectionParameters &where,
                                  const QStringList &columns);
    DataObjects::VariantTable Select(const QString &table_name,
                                  const QStringList &columns);

    // A count query, based on your selection parameter filters
    long Count(const DatabaseSelectionParameters &);

    // A count query that counts all records in the table
    long Count(const QString &table_name);

    // Pass parameters to select out the data you want to update, and
    //  pass values to which to update them
    void Update(const DatabaseSelectionParameters &, const DatabaseValueParameters &);

    void Delete(const DatabaseSelectionParameters &);

    // Call these functions to get a blank table or parameters object that matches the table you declared
    DatabaseSelectionParameters GetBlankSelectionParameters(const QString &table_name) const;
    DatabaseValueParameters GetBlankValueParameters(const QString &table_name) const;
    DataObjects::VariantTable GetBlankTable(const QString &table_name) const;

    // The value returned by the last query
    READONLY_PROPERTY( ReturnValue, QVariant );

    // Is the io device ready to interact with the database?
    bool IsReady() const;


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


protected:

    // You can use these low-level functions if you need to generate
    //  byte array commands of your own, but generally you should use
    //  the convenient functions above
    QByteArray prepare_send_data(
            WriteCommandsEnum cmd,
            const DataObjects::VariantTable &);

    QByteArray prepare_send_data(
            WriteCommandsEnum cmd,
            const DatabaseSelectionParameters &);

    QByteArray prepare_send_data(
            WriteCommandsEnum cmd,
            const DatabaseSelectionParameters &,
            const DatabaseValueParameters &);

    QByteArray prepare_send_data(
            WriteCommandsEnum cmd,
            const DataObjects::VariantTable &,
            const DatabaseSelectionParameters &,
            const DatabaseValueParameters &);


private:

    // Here are the overridden methods from IODevice
    //  Hide these members from the public; nobody should use this interface.
    inline void SendData(const QByteArray &d){ IODevice::SendData(d); }
    inline QByteArray ReceiveData(bool){ return IODevice::ReceiveData(); }

    void send_data(const QByteArray &);
    QByteArray receive_data();

    bool has_data_available();


    // A reference to the QSqlDatabase connection
    QString _connection_id;
    bool _is_connection_open;


    // Control what commands to issue
    ReadCommandsEnum _p_ReadCommand;

    QMap<QString, DataObjects::VariantTable> _tables;

    QList<int> _columns_requested;
    DatabaseSelectionParameters *_selection_parameters;

    inline void _fail_if_not_ready() const{
        if(!IsReady())
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "Database IO device not read for use");
    }

    QString _prepare_where_clause(
            const GUtil::DataObjects::Vector<GUtil::DataObjects::String> &column_keys,
            const DatabaseSelectionParameters &) const;

};



class DatabaseParametersBase :
        public Interfaces::IQXmlSerializable
{
public:

    // Use ColumnOptions to set various options on the columns, like
    //  a different compare operator, NOT/OR it, etc...
    inline GUtil::DataObjects::ResizableCollection
    <DatabaseIODevice::ColumnOptions> &ColumnOptions(){
        return _column_options;
    }
    inline const GUtil::DataObjects::ResizableCollection
    <DatabaseIODevice::ColumnOptions> &ColumnOptions() const{
        return _column_options;
    }

    inline const DataObjects::VariantTable &Table() const{
        return static_cast<const DataObjects::VariantTable &>(_row.Table());
    }

    inline int ColumnCount() const{ return _row.ColumnCount(); }

    inline DatabaseParametersBase(
            const DatabaseParametersBase &p)
        :_column_options(p._column_options),
          _row(p._row)
    {}


protected:

    inline DatabaseParametersBase(const GUtil::DataObjects::DataRow<QVariant> &fv)
        :_column_options(fv.ColumnCount()),
          _row(fv)
    {}

    inline DatabaseParametersBase(){}

    GUtil::DataObjects::ResizableCollection<DatabaseIODevice::ColumnOptions>
    _column_options;

    // Use the Filtervalues to declare a "where" clause, where every
    //  item in the result set matches the FilterValues.
    GUtil::DataObjects::DataRow<QVariant> _row;

    void ReadXml(QXmlStreamReader &);
    void WriteXml(QXmlStreamWriter &) const;

};


class DatabaseSelectionParameters :
        public DatabaseParametersBase
{
    friend class DatabaseIODevice;
public:

    // DataRow is an explicitly shared class, so changes to the
    //   returned object will hold
    inline GUtil::DataObjects::DataRow<QVariant> FilterValues() const{ return _row; }

    inline DatabaseSelectionParameters(const DatabaseParametersBase &p)
        :DatabaseParametersBase(p)
    {}


protected:

    inline DatabaseSelectionParameters(const GUtil::DataObjects::DataRow<QVariant> &r)
        :DatabaseParametersBase(r)
    {}

    inline DatabaseSelectionParameters(){}

};


class DatabaseValueParameters :
        public DatabaseParametersBase
{
    friend class DatabaseIODevice;
public:

    // DataRow is an explicitly shared class, so changes to the
    //   returned object will hold
    inline GUtil::DataObjects::DataRow<QVariant> Values() const{ return _row; }

    inline DatabaseValueParameters(const DatabaseParametersBase &p)
        :DatabaseParametersBase(p)
    {}


protected:

    inline DatabaseValueParameters(const GUtil::DataObjects::DataRow<QVariant> &r)
        :DatabaseParametersBase(r)
    {}

    inline DatabaseValueParameters(){}

};


}}}

#endif // GDATABASEIODEVICE_H

#endif // DATABASE_FUNCTIONALITY
