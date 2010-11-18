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

#include "datatable.h"
#include "collection.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


template <class DataTable> class DataTableCollectionBase :
        public ResizableCollection<DataTable>,
        public Core::Interfaces::IClonable< DataTableCollectionBase<DataTable> >
{
    friend class DataSet;
    friend class TableData;
    friend class SharedSetData;

public:

    DataSet Set() const;


protected:

    DataTableCollectionBase(int size = 0)
        :ResizableCollection<DataTable>(size){}

    DataTableCollectionBase(SharedSetData *d){
        _set_data = d;
    }

    virtual ~DataTableCollectionBase(){}

    DataTableCollectionBase(const DataTableCollectionBase<DataTable> &o){
        o.CloneTo(*this);
    }

    // Protect our clonable interface
    virtual DataTableCollectionBase<DataTable> &CloneTo(DataTableCollectionBase<DataTable> &o) const{
        o.Clear();

        for(int i = 0; i < Collection<DataTable>::Size(); i++)
            o.Add(Collection<DataTable>::At(i).Clone());

        return o;
    }

    virtual void on_add(DataTable *t) const
    {
        SharedSetData *sd = t->table_data().SetData();
        if(_set_data != sd)
        {
            // Remove ourselves from our parent set
            if(sd != 0)
                ((DataTableCollectionBase<DataTable> &)sd->Tables()).RemoveOne(*t);

            // Set our new set data to this one
            t->table_data().SetSetData(_set_data);
        }
    }

    virtual void validate_new_item(const DataTable &t) const
            throw(Core::ValidationException)
    {
        if(Contains(t))
            THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                      "Table already exists in data set");
    }

private:

    SharedSetData *_set_data;

};


GUTIL_END_NAMESPACE

#endif // DATATABLECOLLECTIONBASE_H
