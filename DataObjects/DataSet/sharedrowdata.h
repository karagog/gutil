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

GUTIL_BEGIN_NAMESPACE( DataObjects );


class DataTable;

class SharedRowData :
        public Custom::GSharedData,
        protected UpdatableGVariantCollection
{
public:

    SharedRowData(const DataTable &t,
                  const Custom::GVariantList &vals);

    SharedRowData(const SharedRowData &o);

    virtual ~SharedRowData();


    DataTable &Table();
    const DataTable &Table() const;

    UpdatableGVariantCollection &Tuple();
    const UpdatableGVariantCollection &Tuple() const;


protected:

    virtual void on_value_about_to_change(int index, const Custom::GVariant &newvalue);


private:

    DataTable *_table;

};


GUTIL_END_NAMESPACE

#endif // SHAREDROWDATA_H
