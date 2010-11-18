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

#ifndef DATAROWCOLLECTION_H
#define DATAROWCOLLECTION_H

#include "datatable.h"
#include "collection.h"
#include "Custom/gshareddatapointer.h"


GUTIL_BEGIN_NAMESPACE( DataObjects );


class SharedTableData;

template <class DataRow> class DataRowCollectionBase :
        public Collection<DataRow>,
        public Core::Interfaces::IClonable< DataRowCollectionBase<DataRow> >
{
    friend class DataTable;
    friend class RowData;
    friend class SharedTableData;

public:

    DataTable Table() const{
        return DataTable(_table_data);
    }


protected:

    DataRowCollectionBase(SharedTableData *td){
        _table_data = td;
    }

    virtual ~DataRowCollectionBase(){}

    DataRowCollectionBase(const DataRowCollectionBase<DataRow> &o){
        o.CloneTo(*this);
    }


    // Protect our clonable interface
    virtual DataRowCollectionBase &CloneTo(DataRowCollectionBase<DataRow> &o) const{
        o._table_data = _table_data;

        o.Clear();

        // Clone each row explicitly; each row must detach itself from the shared pointer
        for(int i = 0; i < Collection<DataRow>::Count(); i++)
            o.Add(Collection<DataRow>::At(i).Clone());

        return o;
    }

    virtual void validate_new_item(const DataRow &i) const
            throw(Core::ValidationException)
    {
        if(Table() != i.Table())
            THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                      "The row does not belong to this table.  "
                                      "If you still want to add it, then call 'ImportRow' "
                                      "on the parent table.");
        else if(Collection<DataRow>::Contains(i))
            THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                      "Row already exists in the table");
    }


private:

    SharedTableData *_table_data;

};


// Do make your own derived version of the DataRowCollection class, simply derive from
//   DataRowCollectionBase with your own derived version of DataRow
typedef DataRowCollectionBase<DataRow> DataRowCollection;


GUTIL_END_NAMESPACE


#endif // DATAROWCOLLECTION_H
