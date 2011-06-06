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

#include "DataObjects/updatablegvariantcollection.h"
#include "Custom/gshareddata.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/iupdatable.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class DataTable;

class SharedRowData :
        public Custom::GSharedData,
        public UpdatableGVariantCollection::Observer,
        public Core::Interfaces::IUpdatable
{
public:

    explicit SharedRowData(const DataTable &t,
                           const Custom::GVariantList &vals);

    SharedRowData(const SharedRowData &o);
    SharedRowData();

    virtual ~SharedRowData();


    inline DataTable &Table(){ return *_table; }
    inline const DataTable &Table() const{ return *_table; }

    inline UpdatableGVariantCollection &Tuple(){ return _tuple; }
    inline const UpdatableGVariantCollection &Tuple() const{ return _tuple; }


private:

    DataTable *_table;
    UpdatableGVariantCollection _tuple;


    // UpdatableGVariantCollection::Observer interface:
    void value_about_to_change(int index, const Custom::GVariant &newvalue);

    void on_make_dirty();

};


GUTIL_END_NAMESPACE

#endif // SHAREDROWDATA_H
