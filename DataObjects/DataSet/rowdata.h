/*Copyright Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef ROWDATA_H
#define ROWDATA_H

#include "DataObjects/updatablegvariantcollection.h"
#include "Core/Interfaces/iupdatable.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class TableData;

class RowData :
        public UpdatableGVariantCollection::Observer,
        public Core::Interfaces::IUpdatable
{
    friend class DataRow;
    friend class DataTable;
    friend class DataRowCollection;
public:

    explicit RowData(TableData *,
                     const Custom::GVariantList &vl = Custom::GVariantList());

    RowData(const RowData &o);

    UpdatableGVariantCollection Tuple;
    TableData *Table;


protected:

    // UpdatableGVariantCollection::Observer interface:
    void value_about_to_change(int index, const Custom::GVariant &newvalue);

    // IUpdatable interface:
    void on_make_dirty();

};


GUTIL_END_NAMESPACE

#endif // ROWDATA_H
