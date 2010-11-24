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

#ifndef DATAROWCOLLECTIONBASE_H
#define DATAROWCOLLECTIONBASE_H

#include "collection.h"
#include "datarowcollectionbase.h"


GUTIL_BEGIN_NAMESPACE( DataObjects );


template<class T> class SharedTableData;

template <class RowType>

        class DataRowCollectionBase
            :
            public Collection<RowType>,
            public Core::Interfaces::IClonable< DataRowCollectionBase<RowType> >
{
    template<class T> friend class DataTableBase;
    friend class RowData;
    template<class T> friend class SharedTableData;

    typedef DataTableBase<RowType> TableType;

public:

    TableType &Table(){
        return _table;
    }

    const TableType &Table() const{
        return _table;
    }


    // Allows us to convert to any other kind of datarowcollectionbase
    template <class O> operator DataRowCollectionBase<O>() const{
        return *((DataRowCollectionBase<O> *)this);
    }


protected:

    DataRowCollectionBase(const TableType &t)
        :_table(t){}

    DataRowCollectionBase(const DataRowCollectionBase<RowType> &o){
        o.CloneTo(*this);
    }

    virtual ~DataRowCollectionBase(){
        // It doesn't matter where we put this template guard, because it will fail
        //  at compile time.  We wrap it in an 'if' to suppress the compile error.
        if(RowType::DerivedFromDataRow);
    }

    virtual void validate_new_item(const RowType &i) const
            throw(Core::ValidationException)
    {
        Table().validate_new_row(i);
    }


    // Protect our clonable interface
    virtual DataRowCollectionBase &CloneTo(DataRowCollectionBase<RowType> &o) const{
        o._table = _table;

        o.Clear();

        // Clone each row explicitly; each row must detach itself from the shared pointer
        for(int i = 0; i < Collection<RowType>::Count(); i++)
        {
            TableType dt(_table);
            RowType dr(dt);
            Collection<RowType>::At(i).CloneTo(dr);
            o.Add(dr);
        }

        return o;
    }


private:

    TableType _table;

};


GUTIL_END_NAMESPACE

#endif // DATAROWCOLLECTIONBASE_H
