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

#ifndef PERSONDATAROW_H
#define PERSONDATAROW_H

//#include "DataObjects/datatable.h"
#include "DataObjects/datarow.h"
#include "peopletable.h"
#include "Custom/gvariant.h"
#include <QUuid>

class PersonDataRow;

// We can define a custom data row collection that will specifically hold
//  our derived data row.  You don't have to make one, but it will
//  guarantee that nobody puts a regular DAtaRow into the collection
typedef GUtil::DataObjects::DataRowCollectionBase<PersonDataRow>
        PeopleRowCollection;



class PersonDataRow :
        public GUtil::DataObjects::DataRow
{
    template<class T> friend class GUtil::DataObjects::DataRowCollectionBase;

public:

    PersonDataRow(const PersonDataRow &o)
        :DataRow(o)
    {}

    PersonDataRow(const DataTableBase<PersonDataRow> &tbl,
                  const GUtil::Custom::GVariantList &vals = GUtil::Custom::GVariantList())

            // We pass in our own derivation of the shared data class
        :DataRow(new SharedRowData<PersonDataRow>(tbl, vals))
    {
        // Initialize our Id column, 'cause it's readonly
        At(1) = QUuid::createUuid().toString();
    }


    // With these convenient macros we declare strongly-typed data accessors
    ROW_PROPERTY(Name, QString, "name");
    READONLY_ROW_PROPERTY(Id, QString, "id");


protected:

    // We override these methods which properly cast our row_data as the right
    //  shared data structure
    SharedRowData<PersonDataRow> &row_data(){
        return (SharedRowData<PersonDataRow> &)DataRow::row_data();
    }

    const SharedRowData<PersonDataRow> &row_data() const{
        return (const SharedRowData<PersonDataRow> &)DataRow::row_data();
    }

    // We don't have to implement this method, because our shared row data differs
    //  only in the type of row.  If there was some custom data in the shared struct
    //  then we'd have to manually copy it here, exactly as shown in the comments
//    virtual void copy_shared_data(Custom::GSharedDataPointer<QSharedData> &dest) const{
//        dest = new SharedRowData<PersonDataRow>(row_data());
//    }

};

#endif // PERSONDATAROW_H
