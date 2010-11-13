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

#include "collection.h"
#include "qvariantcollection.h"
#include "Interfaces/iqxmlserializable.h"
#include "Custom/gshareddatapointer.h"
#include "Core/Interfaces/iequatable.h"
#include "Core/Interfaces/iclonable.h"
#include <QVariant>

namespace GUtil
{
    namespace DataObjects
    {
        class DataTable;

        class DataRow : public Interfaces::IQXmlSerializable,
                        public Core::Interfaces::IEquatable<DataRow>,
                        public Core::Interfaces::IClonable<DataRow>
        {
            friend class DataTable;
            friend class DataRowCollection;

        public:

            DataRow(const DataRow &);
            virtual ~DataRow();

            virtual DataRow &operator =(const DataRow &);
            bool operator ==(const DataRow &) const;
            bool operator !=(const DataRow &) const;
            QVariant &operator [](int index);
            QVariant &operator [](const QString &column_header);

            QVariant At(int index) const;

            DataTable &Table();

            int Index() const;
            int ColumnCount() const;


            // IEquatable interface:
            virtual bool Equals(const DataRow &) const;

            // IClonable interface:
            virtual DataRow &CloneTo(DataRow &) const;

            // IQXmlSerializable interface:
            virtual void WriteXml(QXmlStreamWriter &) const;
            virtual void ReadXml(QXmlStreamReader &)
                    throw(Core::XmlException);

        protected:
            DataRow(DataTable *dt = 0, const QVariantList &values = QVariantList());

            void set_table(DataTable *);
            void set_number_of_columns(int);

            class RowData : public QSharedData
            {
            public:
                RowData(DataTable *t, const QVariantList &vals);
                RowData(const RowData &);

                DataTable *table;
                QVariantCollection tuple;

                void SetTable(DataTable *);
            };


            Custom::GSharedDataPointer<RowData> row_data;

        };





        class DataRowCollection :   public Collection<DataRow>,
                                    public Core::Interfaces::IClonable<DataRowCollection>
        {
            friend class DataTable;
            friend class RowData;

        protected:
            DataRowCollection(DataTable *t = 0);

            // Protect our clonable interface
            DataRowCollection(const DataRowCollection &);
            virtual DataRowCollection &CloneTo(DataRowCollection &) const;

            virtual DataRow create_blank_item() const;

        private:
            DataTable *_table;

        };
    }
}

#endif // DATAROW_H
