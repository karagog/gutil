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

#ifndef DATAROW_H
#define DATAROW_H

#include "Interfaces/icollection.h"
#include "Interfaces/iqxmlserializable.h"
#include <QVariant>

namespace GUtil
{
    namespace Custom
    {
        class GSemaphore;
    }

    namespace DataObjects
    {
        class DataTable;
        class DataTuple;

        class DataRow : public Interfaces::IQXmlSerializable
        {
            friend class DataTable;
            friend class DataRowCollection;

        public:
            DataRow();
            DataRow(const DataRow &);
            DataRow &operator =(const DataRow &);

            DataRow Clone() const;

            QVariant &operator [](int index);
            QVariant &operator [](const QString &column_header);

            DataTable &Table();

            int Index() const;
            int ColumnCount() const;

            virtual void WriteXml(QXmlStreamWriter &) const;
            virtual void ReadXml(QXmlStreamReader &)
                    throw(Core::XmlException);

            virtual ~DataRow();

        protected:
            DataRow(DataTable *dt);

            void set_number_of_columns(int);

            DataTable *table;
            int row_index;

        private:
            void _init_data_row(DataTable *);

            DataTuple *_tuple;
            Custom::GSemaphore *_tuple_semaphore;

            void _detach_tuple();
            void _attach_tuple(DataTuple *, Custom::GSemaphore *);
        };





        class DataRowCollection : public Interfaces::ICollection<DataRow>
        {
            friend class DataTable;

        protected:
            DataRowCollection(DataTable *);
            virtual ~DataRowCollection();

            virtual void onAdd(void *, int);

        private:
            DataTable *_table;
        };
    }
}

#endif // DATAROW_H
