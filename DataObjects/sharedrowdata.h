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

#ifndef SHAREDROWDATA_H
#define SHAREDROWDATA_H

#include "datatablebase.h"
#include "gvariantcollection.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


template <class T> class SharedTableData;

template <class T> class SharedRowData :
        public QSharedData
{
public:

    SharedRowData(const DataTable &t, const Custom::GVariantList &vals)
        :tuple(vals){
        SetTableData(&t.table_data());
    }

    SharedRowData(const SharedRowData &o)
        :QSharedData(o),
        tuple(o.tuple){
        SetTableData(o.table_data);
    }

    virtual ~SharedRowData(){}


    SharedTableData<T> *TableData() const{
        return table_data;
    }

    void SetTableData(SharedTableData<T> *t){
        table_data = t;
    }

    GVariantCollection &Tuple(){
        return tuple;
    }

    const GVariantCollection &Tuple() const{
        return tuple;
    }


protected:

    SharedTableData<T> *table_data;
    GVariantCollection tuple;

};


GUTIL_END_NAMESPACE

#endif // SHAREDROWDATA_H
