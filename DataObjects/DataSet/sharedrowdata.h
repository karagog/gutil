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

#include "DataObjects/DataSet/datatable.h"
#include "DataObjects/DataSet/rowvaluecollection.h"
#include "Custom/gshareddata.h"
#include "Core/Interfaces/iupdatable.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class SharedRowData :
        public Custom::GSharedData,
        public Core::Interfaces::IUpdatable
{
public:

    SharedRowData(const DataTable &t,
                  const Custom::GVariantList &vals);

    SharedRowData(const SharedRowData &o);

    virtual ~SharedRowData();


    DataTable &Table();
    const DataTable &Table() const;

    RowValueCollection &Tuple();
    const RowValueCollection &Tuple() const;

    // IUpdatable interface
    bool IsDirty() const;
    void SetDirty(bool);


protected:

    // IUpdatable interface
    void commit_reject_changes(bool commit);

private:

    DataTable *_table;
    RowValueCollection _tuple;

};


GUTIL_END_NAMESPACE

#endif // SHAREDROWDATA_H
