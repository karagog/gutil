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
//  our derived data row.
typedef GUtil::DataObjects::DataRowCollectionBase<PersonDataRow>
        PeopleRowCollection;


// We derive our own SharedRowData object, because we want to include extra data
//   in addition to the data that's stored in the tuple.  Otherwise we could declare
//   this as a typedef of SharedRowDataBase<PersonDataRow>
class SharedPersonData :
        public GUtil::DataObjects::SharedRowDataBase<PersonDataRow>
{
public:

    SharedPersonData(const DataTableBase<PersonDataRow> &t,
                     const Custom::GVariantList &vals)
                         :SharedRowDataBase<PersonDataRow>(t, vals),
                         identifier(QUuid::createUuid()) {}

    SharedPersonData(const SharedPersonData &o)
        :SharedRowDataBase<PersonDataRow>(o),
        identifier(o.identifier) {}

    QUuid Id() const{
        return identifier;
    }


private:

    // Can contain any new custom data; in this case a unique identifier
    QUuid identifier;

};



class PersonDataRow :
        public GUtil::DataObjects::DataRow
{
    template<class T> friend class GUtil::DataObjects::DataRowCollectionBase;
    template<class T> friend class GUtil::DataObjects::DataTableBase;

public:

    PersonDataRow(const PersonDataRow &o)
        :DataRow(o)
    {}

    PersonDataRow(const DataTableBase<PersonDataRow> &tbl,
                  const GUtil::Custom::GVariantList &vals = GUtil::Custom::GVariantList())

            // We pass in our own derivation of the shared data class
        :DataRow(new SharedPersonData(tbl, vals))
    {}


    // With these convenient macros we declare strongly-typed data accessors
    //  Note: You can also use an integer to index the column instead of a string.
    //   The performance would be slightly better, but with a string you don't have
    //   to worry about the order of the columns.
    ROW_PROPERTY(Name, QString, "name");
    ROW_PROPERTY(LastName, QString, "lastname");

    QUuid GetId() const{
        return row_data().Id();
    }


protected:

    // We override these methods which properly cast our row_data as the right
    //  shared data structure
    SharedPersonData &row_data(){
        return (SharedPersonData &)DataRow::row_data();
    }

    const SharedPersonData &row_data() const{
        return (const SharedPersonData &)DataRow::row_data();
    }

    // If there was some custom data in the shared struct
    //  then we'd have to manually copy it here, exactly as shown in the comments
    virtual void copy_shared_data(Custom::GSharedDataPointer<Custom::GSharedData> &dest) const{
        dest = new SharedPersonData(row_data());
    }

};

#endif // PERSONDATAROW_H
