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

#ifndef DATATABLE_H
#define DATATABLE_H

#include "datarow.h"
#include "datarowcollection.h"
#include <QVariantList>
#include <QStringList>
#include <QAbstractTableModel>

namespace GUtil
{
    namespace DataObjects
    {
        class DataSet;

        // A class used to hold data and serialize
        //   the values to xml or access them conveniently with string keys
        class DataTable :   public QAbstractTableModel,
                            public Interfaces::IQXmlSerializable,
                            public Core::Interfaces::IReadOnlyObject,
                            public Core::Interfaces::IUpdatable,
                            public Core::Interfaces::IClonable<DataTable>,
                            public Core::Interfaces::IEquatable<DataTable>
        {
            Q_OBJECT

            friend class DataSet;
            friend class DataTableCollection;
            friend class DataRow;
            friend class DataRowCollection;
            friend class SharedRowData;

        public:

            DataTable(int num_cols = 0);
            DataTable(const QString &table_name, int num_cols = 0);
            DataTable(const DataTable &);
            virtual ~DataTable();

            // The row must have been created by this table, otherwise call ImportRow
            DataRow AddRow(const DataRow &);

            DataRow AddNewRow(const QVariantList &values = QVariantList());
            DataRow CreateRow(const QVariantList &values = QVariantList());

            // Clones the row and adds the clone to this table
            DataRow ImportRow(const DataRow &);

            void RemoveRow(int row_index);
            void RemoveRow(const DataRow &);

            // Clears all data and columns
            void Clear();

            void AddColumn(const QString &key, const QString &label = QString::null);
            void SetColumnHeaders(const QStringList &keys, const QStringList &labels = QStringList());
            void SetColumnLabel(int col_index, const QString &);
            void SetColumnKey(int col_index, const QString &);
            void ClearColumns();

            int RowCount() const;
            int ColumnCount() const;

            int GetColumnIndex(const QString &key) const;

            DataRow &operator [](int);
            DataTable &operator =(const DataTable &);
            bool operator ==(const DataTable &) const;
            bool operator !=(const DataTable &) const;

            DataSet Set() const;
            DataRowCollection &Rows();
            QStringList ColumnKeys() const;
            QStringList ColumnLabels() const;
            QString Name() const;
            void SetTableName(const QString &) const;

            DataTable Clone() const;


            // Interface for IEquatable:
            virtual bool Equals(const DataTable &) const;

            // Interface for IQXmlSerializable
            virtual void WriteXml(QXmlStreamWriter &) const;
            virtual void ReadXml(QXmlStreamReader &)
                    throw(GUtil::Core::XmlException);

            virtual void SetReadOnly(bool readonly);


            // Interface for QAbstractTableModel
            virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
            virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

            virtual QVariant data(const QModelIndex &index, int role) const;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

            virtual bool insertRows(int row, int count, const QModelIndex &parent);
            virtual bool removeRows(int row, int count, const QModelIndex &parent);

            virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
            virtual Qt::ItemFlags flags(const QModelIndex &index) const;


        protected:

            DataTable(const DataSet &);
            DataTable(SharedTableData *);

            // Friend classes can access our data through this method:
            SharedTableData &table_data() const;

            virtual DataTable &CloneTo(DataTable &) const;

            virtual void commit_reject_changes(bool commit);


        private:

            void _init(const QString &name, int num_cols);

            Custom::GSharedDataPointer<SharedTableData> _table_data;

        };
    }
}

#endif // DATATABLE_H
