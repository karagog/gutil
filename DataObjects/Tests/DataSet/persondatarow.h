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

#include "DataObjects/datarow.h"
#include <QUuid>


class PersonDataRow;

// We can define a custom data row collection that will specifically hold
//  our derived data row.  You don't have to make one, but it will
//  guarantee that nobody puts a regular DAtaRow into the collection
typedef GUtil::DataObjects::DataRowCollectionBase<PersonDataRow>
        PersonDataRowCollection;


// We don't have to create a derived version of the shared data, but
//  the framework is extensible like this
class SharedPersonData : public GUtil::DataObjects::SharedRowData
{
public:

    SharedPersonData(const SharedPersonData &o)
        :SharedRowData(o),
        identifier(o.identifier)
    {}

    SharedPersonData(SharedTableData *td, const QVariantList &vals)
        :SharedRowData(td, vals),
        identifier(QUuid::createUuid())
    {}



    // Some extra data you want to store
    QUuid identifier;
};





class PersonDataRow :
        public GUtil::DataObjects::DataRow
{
public:

    PersonDataRow(const PersonDataRow &o)
        :DataRow(o)
    {}

    PersonDataRow(const DataTable &tbl, const QVariantList &vals = QVariantList())

            // We pass in our own derivation of the shared data class
        :DataRow(new SharedPersonData(&tbl.table_data(), vals))
    {}


    // We can create our own custom accessors to the data
    PROPERTY_ACCESSORS( Name, QString );
    PROPERTY_ACCESSORS( LastName, QString );

    READONLY_PROPERTY_ACCESSORS( Id, QUuid );


protected:

    // We override this function to access the derived class
    SharedPersonData &row_data() const{
        return (SharedPersonData &)DataRow::row_data();
    }

};

#endif // PERSONDATAROW_H
