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
#include "DataObjects/datatuple.h"
#include <QVariant>
#include <QUuid>
#include <QExplicitlySharedDataPointer>

namespace GUtil
{
    namespace Custom
    {
        class GSemaphore;
    }

    namespace DataObjects
    {
        class DataTable;

        class DataRow : public Interfaces::IQXmlSerializable
        {
            friend class DataTable;
            friend class DataRowCollection;

        public:
            DataRow();
            DataRow(const DataRow &);
            virtual ~DataRow();

            DataRow Clone() const;

            DataRow &operator =(const DataRow &);
            bool operator ==(const DataRow &) const;
            bool operator !=(const DataRow &) const;
            QVariant &operator [](int index);
            QVariant &operator [](const QString &column_header);

            QVariant At(int index) const;

            static bool Equal(const DataRow &, const DataRow &);

            DataTable &Table();

            int Index() const;
            int ColumnCount() const;

            virtual void WriteXml(QXmlStreamWriter &) const;
            virtual void ReadXml(QXmlStreamReader &)
                    throw(Core::XmlException);

        protected:
            DataRow(DataTable *dt);

            void set_table(DataTable *);
            void set_number_of_columns(int);

            class RowData : public QSharedData
            {
            public:
                RowData(DataTable *t = 0);
                RowData(const RowData &);

                DataTable *table;
                DataTuple tuple;
                QUuid identifier;
            };


            QExplicitlySharedDataPointer<RowData> row_data;

        private:
            void _init_data_row(DataTable *);

        };





        class DataRowCollection : public Interfaces::ICollection<DataRow>
        {
            friend class DataRow;
            friend class DataTable;

        protected:
            DataRowCollection(DataTable *);
            virtual ~DataRowCollection();

            // Returns -1 if not found
            int find_row_by_id(const QUuid &row_id);

            virtual void onAdd(void *, int);

        private:
            DataTable *_table;
        };
    }
}

#endif // DATAROW_H
