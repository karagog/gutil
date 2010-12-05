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

#ifndef DATATABLE_H
#define DATATABLE_H

#include "DataObjects/DataSet/datarow.h"
#include "DataObjects/explicitlysharedobject.h"
#include "Custom/gvariant.h"
#include "Interfaces/iqxmlserializable.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/iclonable.h"
#include "Core/Interfaces/iequatable.h"
#include <QVariantList>
#include <QStringList>
#include <QReadWriteLock>


#define DECLARE_DATATABLE_ROW_TYPE( RowType ) \
    inline RowType &operator [](int index){ return (RowType &)Rows()[index]; } \
    inline const RowType &operator [](int index) const{ return (const RowType &)Rows()[index]; } \
    inline RowType &At(int index){ return (RowType &)Rows()[index]; } \
    inline const RowType &At(int index) const{ return (const RowType &)Rows()[index]; } \
    \
    RowType &AddRow(const RowType &r){ return (RowType &)add_row(r); } \
    RowType &AddNewRow(const Custom::GVariantList &values = Custom::GVariantList()) \
        { return (RowType &)add_new_row(values); } \
    RowType &ImportRow(const RowType &r){ return (RowType &)import_row(r); } \
    enum{}




GUTIL_BEGIN_NAMESPACE( DataObjects );


class DataSet;
class SharedTableData;

// A Data Table class used to hold data and serialize
//   the values to xml or access them conveniently with string keys

class DataTable
            :
            public ExplicitlySharedObject< SharedTableData >,
            public Interfaces::IQXmlSerializable,
            public Core::Interfaces::IReadOnlyObject,
            public Core::Interfaces::IUpdatable,
            public Core::Interfaces::IClonable< DataTable >,
            public Core::Interfaces::IEquatable< DataTable >
{
    friend class DataSet;
    friend class DataTableCollection;
    friend class DataRow;
    friend class SharedRowData;
    friend class DataRowCollection;
    friend class SharedTableData;

public:

    // This declares all the accessors for the data row type
    DECLARE_DATATABLE_ROW_TYPE( DataRow );

    DataTable();
    DataTable(const QString &table_name, int num_cols = 0);
    DataTable(const DataTable &);
    virtual ~DataTable();

    void RemoveRow(int row_index);
    void RemoveRow(const DataRow &);

    // Clears all data and columns
    void Clear();

    void AddColumn(const QString &key, const QString &label = QString::null);
    void SetColumnHeaders(const QStringList &keys, const QStringList &labels = QStringList());
    void SetColumnLabel(int col_index, const QString &);
    void SetColumnKey(int col_index, const QString &);
    void ClearColumns();

    QSet<int> KeyColumns() const;
    void AddKeyColumn(int);
    void AddKeyColumn(const QString &);
    void RemoveKeyColumn(int);
    void RemoveKeyColumn(const QString &);

    int RowCount() const;
    int ColumnCount() const;

    int GetColumnIndex(const QString &key) const;


    DataRowCollection &Rows();
    const DataRowCollection &Rows() const;


    DataTable &operator =(const DataTable &);
    DataSet Set() const;

    QStringList ColumnKeys() const;
    QStringList ColumnLabels() const;
    QString Name() const;
    void SetTableName(const QString &);

    DataTable Clone() const;


    // Find a row based on one key column
    DataRow &FindRow(int, const Custom::GVariant &);
    const DataRow &FindRow(int, const Custom::GVariant &) const;

    // Find a row based on multiple key columns
    DataRow &FindRow(const QMap<int, Custom::GVariant> &keycolumn_value_mapping)
            throw(Core::NotFoundException);
    const DataRow &FindRow(const QMap<int, Custom::GVariant> &keycolumn_value_mapping) const
            throw(Core::NotFoundException);

    void LockForRead();
    void LockForWrite();
    bool TryLockForRead();
    bool TryLockForWrite();
    void Unlock();

    bool IsDirty() const;

    // Interface for IEquatable:
    virtual bool Equals(const DataTable &) const;

    // Interface for IQXmlSerializable
    virtual void WriteXml(QXmlStreamWriter &) const;
    virtual void ReadXml(QXmlStreamReader &)
            throw(GUtil::Core::XmlException);


protected:

    DataTable(const DataSet &,
                  const QString &name = QString::null,
                  int num_cols = 0);

    // Derived classes can call this constructor with their own derived
    //  version of the shared data object
    DataTable(SharedTableData *);

    // Derived tables can implement their own row initializations, like incrementing
    //  an id or initializing a GUID
    virtual void init_new_row(DataRow &){}

    // Friend classes can access our data through this method:
    SharedTableData &table_data();
    const SharedTableData &table_data() const;

    DataRow &add_new_row(const Custom::GVariantList &);
    DataRow &add_row(const DataRow &);
    DataRow &import_row(const DataRow &);

    // This is called any time you add a new row to the table
    void validate_new_row(const DataRow &r)
            throw(Core::ValidationException);

    // IClonable Interface:
    virtual DataTable &CloneTo(DataTable &) const;

    // IUpdatable interface:
    virtual void on_make_dirty();
    virtual void commit_reject_changes(bool commit);


private:

    void _init(const QString &name, int num_cols);

    bool _key_violations() const;

};


GUTIL_END_NAMESPACE

#endif // DATATABLE_H
