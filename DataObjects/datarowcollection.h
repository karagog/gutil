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

namespace GUtil
{
    namespace DataObjects
    {
        class SharedTableData;

        class DataRowCollection :
                public Collection<DataRow *>,
                public Core::Interfaces::IClonable<DataRowCollection>
        {
            friend class DataTable;
            friend class RowData;
            friend class SharedTableData;

        public:

            DataTable Table() const;

            DataRow &At(int) const;

        protected:

            DataRowCollection(SharedTableData *);

            // Protect our clonable interface
            DataRowCollection(const DataRowCollection &);
            virtual DataRowCollection &CloneTo(DataRowCollection &) const;

            virtual DataRow *create_blank_item();

            virtual void validate_new_item(const DataRow * const) const
                    throw(Core::ValidationException);


        private:

            // Hide this function
            virtual inline DataRow *Value(int index) const
                    throw(Core::IndexOutOfRangeException){
                return Collection<DataRow *>::Value(index);
            }

            SharedTableData *_table_data;

        };
    }
}

#endif // DATAROWCOLLECTION_H
