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

#include "qvarianthelpers.h"
#include "Interfaces/iqxmlserializable.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include <QAbstractItemModel>
#include <QVariantList>
#include <QPointer>
#include <QStringList>

namespace GUtil
{
    namespace DataObjects
    {
        class DataTable;

        class DataSet : public QAbstractItemModel,
                        public GUtil::Interfaces::IQXmlSerializable,
                        public GUtil::Core::Interfaces::IReadOnlyObject
        {
            Q_OBJECT

            friend class DataTable;

        public:
            DataSet(QObject *parent = 0);
            virtual ~DataSet();

            QPointer<DataTable> AddTable(int column_count = 0);
            QPointer<DataTable> AddTable(const QStringList &col_keys,
                                         const QStringList &col_labels = QStringList());

            // Should we commit changes right away, or wait 'til someone calls 'CommitChanges'?
            void SetAutoCommitChanges(bool);
            bool AutoCommitChanges() const;

            void CommitChanges();
            void RejectChanges();

            bool Dirty() const;

            // IQXmlSerializable
            void WriteXml(QXmlStreamWriter &) const;
            void ReadXml(QXmlStreamReader &)
                    throw(GUtil::Core::XmlException);

        protected:

            // Friend classes can use these functions to access the backend datastore
            void set_value(int table_index, int row_index, int col_index, const QVariant &value);
            QVariant get_value(int table_index, int row_index, int col_index) const;

            void set_row_values(int table_index, int row_index, const QVariantList &values);
            QVariantList get_row_values(int table_index, int row_index) const;

            void set_table_values(int table_index, const QList<QVariantList> &values);
            QList<QVariantList> get_table_values(int table_index) const;

            QStringList get_column_keys(int table_index) const;

            // Derived classes can override this function to do special things when you commit data
            virtual void commit_reject_changes(bool commit);

        private:
            QList< QList< QList< QVariant > > > data;
            QList< QList< QList< QVariant > > > _orig_data;
            QList< QPointer<DataTable> > _tables;

            // Column keys/labels for each corresponding table
            QList<QStringList> _keys;
            QList<QStringList> _labels;

            QString _get_label(int table, int column) const;

            bool _auto_commit;
            bool _dirty;
        };
    }
}

#endif // ABSTRACTDATATUPLE_H
