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

#ifndef DATATABLEBASE_H
#define DATATABLEBASE_H

#include "datarow.h"
#include "datatable.h"
#include "sharedtabledata.h"
#include "explicitlysharedobject.h"
#include "Custom/gvariant.h"
#include "Interfaces/iqxmlserializable.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/iclonable.h"
#include "Core/Interfaces/iequatable.h"
#include <QVariantList>
#include <QStringList>

GUTIL_BEGIN_NAMESPACE( DataObjects );


class DataSet;
class DataRow;

// A Data Table class used to hold data and serialize
//   the values to xml or access them conveniently with string keys

template <class RowType>

        class DataTableBase
            :
            public ExplicitlySharedObject< SharedTableData<RowType> >,
            public Interfaces::IQXmlSerializable,
            public Core::Interfaces::IReadOnlyObject,
            public Core::Interfaces::IUpdatable,
            public Core::Interfaces::IClonable< DataTableBase<RowType> >,
            public Core::Interfaces::IEquatable< DataTableBase<RowType> >
{
    friend class DataSet;
    friend class DataTableCollection;
    friend class DataRow;
    template <class T> friend class DataRowCollectionBase;
    template <class T> friend class SharedRowDataBase;
    template <class T> friend class SharedTableData;

public:

    DataTableBase(int num_cols = 0);
    DataTableBase(const QString &table_name, int num_cols = 0);
    DataTableBase(const DataTableBase<RowType> &);
    virtual ~DataTableBase();

    RowType &AddRow(const RowType &);
    RowType &AddNewRow(const Custom::GVariantList &values = Custom::GVariantList());

    // Clones the row and adds the clone to this table
    RowType & ImportRow(const RowType &);

    void RemoveRow(int row_index);
    void RemoveRow(const RowType &);

    // Clears all data and columns
    void Clear();

    void AddColumn(const QString &key, const QString &label = QString::null);
    void SetColumnHeaders(const QStringList &keys, const QStringList &labels = QStringList());
    void SetColumnLabel(int col_index, const QString &);
    void SetColumnKey(int col_index, const QString &);
    void ClearColumns();

    int RowCount() const;
    int ColumnCount() const;

    int GetColumnIndex(const QString &key) const;


    // These functions can be overridden to return a derived version of datarow/collection
    RowType &operator [](int);
    const RowType &operator [](int) const;

    DataRowCollectionBase<RowType> &Rows();
    const DataRowCollectionBase<RowType> &Rows() const;


    DataTableBase &operator =(const DataTableBase<RowType> &);

    // Use to cast any type of table to this kind of datatable base
    template <class O> operator const DataTableBase<O> &() const{
        return *((const DataTableBase<O> *)this);
    }
    template <class O> operator DataTableBase<O> &(){
        return *((DataTableBase<O> *)this);
    }

    DataSet Set() const;

    QStringList ColumnKeys() const;
    QStringList ColumnLabels() const;
    QString Name() const;
    void SetTableName(const QString &);

    DataTableBase<RowType> Clone() const;


    // Interface for IEquatable:
    virtual bool Equals(const DataTableBase &) const;

    // Interface for IQXmlSerializable
    virtual void WriteXml(QXmlStreamWriter &) const;
    virtual void ReadXml(QXmlStreamReader &)
            throw(GUtil::Core::XmlException);


protected:

    DataTableBase(const DataSet &,
                  const QString &name = QString::null,
                  int num_cols = 0);

    // Derived classes can call this constructor with their own derived
    //  version of the shared data object
    DataTableBase(SharedTableData<RowType> *);

    // Derived tables can implement their own row initializations
    virtual void init_new_row(DataRow &){}

    // Friend classes can access our data through this method:
    SharedTableData<RowType> &table_data();
    const SharedTableData<RowType> &table_data() const;

    // This is called any time you add a new row to the table
    void validate_new_row(const DataRow &r)
            throw(Core::ValidationException)
    {
        if(*this != r.row_data().Table())
            THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                      "The row does not belong to this table.  "
                                      "If you still want to add it, then call 'ImportRow' "
                                      "on the parent table.");
        else if(Rows().Contains(r))
            THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                      "Row already exists in the table");

        // Derived tables can provide extra validation
        table_data().ValidateNewRow(*this, r);
    }

    // IClonable Interface:
    virtual DataTableBase<RowType> &CloneTo(DataTableBase<RowType> &) const;

    // IUpdatable interface:
    virtual void commit_reject_changes(bool commit);


private:

    void _init(const QString &name, int num_cols);

};


GUTIL_END_NAMESPACE


// Keep the implementation in a separate file
#include "datatablebase.cpp"

#endif // DATATABLEBASE_H
