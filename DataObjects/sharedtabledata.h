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
        SetSetData(0);
    }

    SharedTableData(const DataSet &ds)
        :_rows(new DataRowCollectionBase<RowType>(this))
    {
        SetSetData(&ds.set_data());
    }

    SharedTableData(const SharedTableData &d)
        :_rows(new DataRowCollectionBase<RowType>(d.Rows())),
        _columns(d.Columns())
    {
        SetSetData(d._set_data);
    }

    virtual ~SharedTableData(){
        delete _rows;
    }

    SharedSetData *SetData() const{
        return _set_data;
    }

    void SetSetData(SharedSetData *d){
        _set_data = d;
    }

    DataRowCollectionBase<RowType> &Rows() const{
        return *_rows;
    }

    DataColumnCollection &Columns(){
        return _columns;
    }

    const DataColumnCollection &Columns() const{
        return _columns;
    }


protected:

    SharedSetData *_set_data;
    DataRowCollectionBase<RowType> *_rows;
    DataColumnCollection _columns;
    QString name;

};


GUTIL_END_NAMESPACE

#endif // SHAREDTABLEDATA_H
