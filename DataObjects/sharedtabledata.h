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

#ifndef SHAREDTABLEDATA_H
#define SHAREDTABLEDATA_H

#include "dataset.h"
#include "datacolumncollection.h"
#include "datarowcollectionbase.h"
#include "Custom/gshareddata.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


template <class RowType> class SharedTableData :
        public Custom::GSharedData
{
    template <class T> friend class DataTableBase;

public:

    SharedTableData()
        :_rows(new DataRowCollectionBase<RowType>(this))
    {
        validate_new_row_custom = 0;
    }

    SharedTableData(const DataSet &ds)
        :_dataset(ds),
        _rows(new DataRowCollectionBase<RowType>(this))
    {
        validate_new_row_custom = 0;
    }

    SharedTableData(const SharedTableData &d)
        :_dataset(d._dataset),
        _rows(new DataRowCollectionBase<RowType>(d.Rows())),
        _columns(d.Columns())
    {
        validate_new_row_custom = d.validate_new_row_custom;
    }

    virtual ~SharedTableData(){
        delete _rows;
    }

    DataSet GetDataSet() const{
        return DataSet(_dataset);
    }

    void SetDataSet(const DataSet &ds){
        _dataset = DataSet(ds);
    }

    const DataRowCollectionBase<RowType> &Rows() const{
        return *_rows;
    }

    DataRowCollectionBase<RowType> &Rows(){
        return *_rows;
    }

    DataColumnCollection &Columns(){
        return _columns;
    }

    const DataColumnCollection &Columns() const{
        return _columns;
    }


    QString GetName() const{
        return _name;
    }

    void SetName(const QString &v){
        _name = v;
    }


    // Derived tables can set their own row validation function:
    void (*validate_new_row_custom)(const DataTable &, const DataRow &)
            throw(Core::ValidationException);


private:

    DataSet _dataset;
    DataRowCollectionBase<RowType> *_rows;
    DataColumnCollection _columns;
    QString _name;

};


GUTIL_END_NAMESPACE

#endif // SHAREDTABLEDATA_H
