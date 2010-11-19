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

#include "DataObjects/datatable.h"
#include <QUuid>


class PersonDataRow;

// We can define a custom data row collection that will specifically hold
//  our derived data row.  You don't have to make one, but it will
//  guarantee that nobody puts a regular DAtaRow into the collection
typedef GUtil::DataObjects::DataRowCollectionBase<PersonDataRow>
        PersonDataRowCollection;





class PersonDataRow :
        public GUtil::DataObjects::DataRow
{
public:

    PersonDataRow(const PersonDataRow &o)
        :DataRow(o)
    {}

    PersonDataRow(const GUtil::DataObjects::DataTable &tbl,
                  const GUtil::Custom::GVariantList &vals = GUtil::Custom::GVariantList())

            // We pass in our own derivation of the shared data class
        :DataRow(tbl, vals)
    {
        // Initialize our Id column, 'cause it's readonly
        At(1) = QUuid::createUuid().toString();
    }


    // With these convenient macros we declare strongly-typed data accessors
    ROW_PROPERTY(Name, QString, 0);
    READONLY_ROW_PROPERTY(Id, QString, 1);

};

#endif // PERSONDATAROW_H
