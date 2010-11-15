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

#include "shareddataobjects.h"
#include "Interfaces/iqxmlserializable.h"
#include "Custom/gshareddatapointer.h"
#include "Core/Interfaces/iequatable.h"
#include "Core/Interfaces/iclonable.h"
#include <QVariant>

namespace GUtil
{
    namespace DataObjects
    {
        // Defines a row in a data table

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

            DataRow Clone() const;

            // IEquatable interface:
            virtual bool Equals(const DataRow &) const;


            // IQXmlSerializable interface:
            virtual void WriteXml(QXmlStreamWriter &) const;
            virtual void ReadXml(QXmlStreamReader &)
                    throw(Core::XmlException);


        protected:

            DataRow(DataTable *dt, const QVariantList &values = QVariantList());

            // Friend classes can access our data via this method
            SharedRowData &row_data() const;

            // IClonable interface:
            virtual DataRow &CloneTo(DataRow &) const;

            void set_number_of_columns(int);


        private:

            Custom::GSharedDataPointer<SharedRowData> _row_data;

        };
    }
}

#endif // DATAROW_H
