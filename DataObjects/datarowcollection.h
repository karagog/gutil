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
                public CollectionBase<DataRow *>,
                public Core::Interfaces::IClonable<DataRowCollection>
        {
            friend class DataTable;
            friend class RowData;
            friend class SharedTableData;

        public:

            DataTable Table() const;

            DataRow &At(int) const;

            DataRow &Add(const DataRow &);
            DataRow &Insert(const DataRow &, int index);

            int Count() const;
            int Size() const;

            bool Contains(const DataRow &) const;
            int IndexOf(const DataRow &) const;

            void Remove(int);
            void RemoveOne(const DataRow &);
            void RemoveAll(const DataRow &);

            void Clear();
            void Resize(int);

            DataRow &operator [](int);
            const DataRow &operator [](int) const;

        protected:

            DataRowCollection(SharedTableData *);

            // Protect our clonable interface
            DataRowCollection(const DataRowCollection &);
            virtual DataRowCollection &CloneTo(DataRowCollection &) const;

            virtual DataRow *create_blank_item();

            virtual void validate_new_item(const DataRow * const) const
                    throw(Core::ValidationException);

            virtual void on_remove(DataRow **) const;

            virtual bool compare_equality(const DataRow *const,
                                          const DataRow *const) const;


        private:

            SharedTableData *_table_data;

        };
    }
}

#endif // DATAROWCOLLECTION_H
