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

#ifndef DATATABLECOLLECTIONBASE_H
#define DATATABLECOLLECTIONBASE_H

#include "datatablebase.h"
#include "collection.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class DataSet;
class SharedSetData;

class DataTableCollection :
        public ResizableCollection<DataTable>,
        public Core::Interfaces::IClonable<DataTableCollection>
{
    friend class DataSet;
    friend class TableData;
    friend class SharedSetData;

public:

    DataSet &Set();
    const DataSet &Set() const;

    ~DataTableCollection();


protected:

    DataTableCollection(SharedSetData *d, int size = 0);

    DataTableCollection(const DataTableCollection &o){
        o.CloneTo(*this);
    }


    // Protect our clonable interface
    virtual DataTableCollection &CloneTo(DataTableCollection &o) const;

    virtual void on_add(DataTable *t);

    virtual void validate_new_item(const DataTable &t)
            throw(Core::ValidationException);

private:

    DataSet *_dataset;

};


GUTIL_END_NAMESPACE

#endif // DATATABLECOLLECTIONBASE_H
