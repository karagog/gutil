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

#ifndef ABSTRACTDATATUPLE_H
#define ABSTRACTDATATUPLE_H

#include "datatable.h"
#include "collection.h"
#include "qvarianthelpers.h"
#include "Custom/gshareddatapointer.h"
#include "Interfaces/iqxmlserializable.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include <QAbstractItemModel>
#include <QVariantList>
#include <QStringList>

namespace GUtil
{
    namespace DataObjects
    {
        class DataSet : public QAbstractItemModel,
                        public Interfaces::IQXmlSerializable,
                        public Core::Interfaces::IReadOnlyObject,
                        public Core::Interfaces::IUpdatable,
                        public Core::Interfaces::IEquatable<DataSet>,
                        public Core::Interfaces::IClonable<DataSet>
        {
            Q_OBJECT

            friend class DataTable;

        public:

            DataSet(QObject *parent = 0);
            DataSet(const DataSet &);
            virtual ~DataSet();

            DataTableCollection &Tables();
            int TableCount() const;
            DataTable &AddTable(const DataTable &);
            void Clear();

            DataSet &operator =(const DataSet &);
            DataTable &operator [](int);
            DataTable &operator [](const QString &table_name);
            bool operator ==(const DataSet &) const;
            bool operator !=(const DataSet &) const;

            int GetTableIndex(const QString &table_name) const;

            DataSet Clone() const;


            // IEquatable interface:
            virtual bool Equals(const DataSet &) const;

            // IQXmlSerializable
            void WriteXml(QXmlStreamWriter &) const;
            void ReadXml(QXmlStreamReader &)
                    throw(GUtil::Core::XmlException);

            // QAbstractItemModel:
            virtual QModelIndex index(int, int, const QModelIndex &) const;
            virtual QModelIndex parent(const QModelIndex &child) const;

            virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

            virtual QVariant data(const QModelIndex &index, int role) const;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

            virtual bool insertRows(int row, int count, const QModelIndex &parent);
            virtual bool removeRows(int row, int count, const QModelIndex &parent);

            virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
            virtual Qt::ItemFlags flags(const QModelIndex &index) const;


        protected:

            SharedSetData &set_data() const;

            // IUpdatable interface:
            virtual void commit_reject_changes(bool commit);

            // IClonable interface:
            virtual DataSet &CloneTo(DataSet &) const;


        private:

            Custom::GSharedDataPointer<SharedSetData> _set_data;

        };
    }
}

#endif // ABSTRACTDATATUPLE_H
