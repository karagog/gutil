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
#include "Custom/gshareddata.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class SharedRowData :
        public Custom::GSharedData
{
public:

    SharedRowData(const DataTable &t,
                  const Custom::GVariantList &vals)
        :_table(t),
        tuple(vals)
    {
        tuple.Resize(_table.ColumnCount());
    }

    SharedRowData(const SharedRowData &o)
        :Custom::GSharedData(o),
        _table(o._table),
        tuple(o.tuple){}

    virtual ~SharedRowData(){}


    DataTable &Table(){
        return _table;
    }

    const DataTable &Table() const{
        return _table;
    }

    GVariantCollection &Tuple(){
        return tuple;
    }

    const GVariantCollection &Tuple() const{
        return tuple;
    }


private:

    DataTable _table;
    GVariantCollection tuple;

};


GUTIL_END_NAMESPACE

#endif // SHAREDROWDATA_H
