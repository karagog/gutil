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
#include "collection.h"
#include "Custom/gshareddatapointer.h"
#include "Interfaces/iqxmlserializable.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/ireadonlyobject.h"
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
                            public Core::Interfaces::IClonable<DataTable>
        {
            Q_OBJECT

            friend class DataTableCollection;

        protected:

            // All the table's data is contained in this shared data class
            class TableData : public QSharedData
            {
            public:
                TableData(DataTable *parent_table);
                TableData(const TableData &);

                DataSet *dataset;
                DataRowCollection rows;
                QStringList keys;
                QStringList labels;
                QString name;
            };

            // Friend classes can access the data through this method:
            TableData &table_data() const;


        public:

            DataTable(int num_cols = 0);
            DataTable(const QString &table_name, int num_cols = 0);
            DataTable(const DataTable &);
            virtual ~DataTable();

            // The row must have been created by this table, otherwise call ImportRow
            DataRow AddRow(const DataRow &)
                    throw(Core::ArgumentException);

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
            void SetColumnLabels(const QStringList &);
            void SetColumnKey(int col_index, const QString &);
            void SetColumnKeys(const QStringList &);
            void ClearColumns();

            int RowCount() const;
            int ColumnCount() const;

            int GetColumnIndex(const QString &key) const;

            DataRow &operator [](int);
            DataTable &operator =(const DataTable &);
            bool operator ==(const DataTable &) const;

            DataSet *DataSetParent() const;
            DataRowCollection &Rows();
            QStringList ColumnKeys() const;
            QStringList ColumnLabels() const;
            QString Name() const;
            void SetTableName(const QString &) const;


            // Interface for IClonable:
            virtual DataTable &CloneTo(DataTable &) const;

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

            DataTable(DataSet *ds);


        private:

            void _init(DataSet *, const QString &, int);

            Custom::GSharedDataPointer<TableData> _table_data;

        };


        class DataTableCollection : public Collection<DataTable>,
                                    public Core::Interfaces::IClonable<DataTableCollection>
        {
            friend class DataSet;
            friend class TableData;

        protected:

            DataTableCollection(DataSet *);

            // Protect our clonable interface
            DataTableCollection(const DataTableCollection &);
            virtual DataTableCollection &CloneTo(DataTableCollection &) const;

            virtual void on_add(DataTable &) const;
            virtual DataTable create_blank_item() const;


        private:

            DataSet *_dataset;

        };
    }
}

#endif // DATATABLE_H
