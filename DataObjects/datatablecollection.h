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

#ifndef DATATABLECOLLECTION_H
#define DATATABLECOLLECTION_H

#include "datatable.h"

namespace GUtil
{
    namespace DataObjects
    {
        class SharedSetData;

        class DataTableCollection : public Collection<DataTable>,
                                    public Core::Interfaces::IClonable<DataTableCollection>
        {
            friend class DataSet;
            friend class TableData;
            friend class SharedSetData;

        protected:

            DataTableCollection(SharedSetData *);

            // Protect our clonable interface
            DataTableCollection(const DataTableCollection &);
            virtual DataTableCollection &CloneTo(DataTableCollection &) const;

            virtual void on_add(DataTable &) const;
            virtual DataTable create_blank_item() const;

            virtual void validate_new_item(const DataTable &) const
                    throw(Core::ValidationException);


        private:

            SharedSetData *_set_data;

        };
    }
}

#endif // DATATABLECOLLECTION_H
