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

#ifndef DATAROWCOLLECTION_H
#define DATAROWCOLLECTION_H

#include "datarow.h"
#include "DataObjects/collection.h"
#include "Core/Interfaces/iclonable.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class SharedTableData;

class DataRowCollection :
        public Collection<DataRow>
{
public:

    explicit DataRowCollection(TableData *);

    // This deep copy constructor clones the entire table
    DataRowCollection(const DataRowCollection &);

    // This is a deep-copy constructor; but you need to provide it with a new
    //  data table
    explicit DataRowCollection(TableData *, const DataRowCollection &o);


protected:

    virtual void validate_new_item(const DataRow &i)
            throw(Core::ValidationException<true>);


private:

    TableData *_table;

    void _init_cloned_rows(const DataRowCollection &);

};


GUTIL_END_NAMESPACE

#endif // DATAROWCOLLECTION_H
