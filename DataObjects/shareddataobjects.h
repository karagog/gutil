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

#ifndef SHAREDDATAOBJECTS_H
#define SHAREDDATAOBJECTS_H

#include <QSharedData>
#include <QString>
#include <QVariantList>

namespace GUtil
{
    namespace DataObjects
    {
        class DataSet;
        class DataTable;
        class DataTableCollection;
        class DataColumnCollection;
        class DataRow;
        class DataRowCollection;

        class QVariantCollection;


        class SharedSetData : public QSharedData
        {
        public:

            SharedSetData(DataSet *);
            SharedSetData(const SharedSetData &);
            ~SharedSetData();

            DataTableCollection &Tables() const;


        private:

            DataTableCollection *_tables;

        };



        class SharedTableData : public QSharedData
        {
        public:

            SharedTableData(SharedSetData *parent_data);
            SharedTableData(const SharedTableData &);
            ~SharedTableData();

            SharedSetData *SetData() const;
            void SetSetData(SharedSetData *);

            DataRowCollection &Rows() const;
            DataColumnCollection &Columns() const;
            QString name;


        private:

            SharedSetData *_set_data;
            DataRowCollection *_rows;
            DataColumnCollection *_columns;

        };



        class SharedRowData : public QSharedData
        {
        public:

            SharedRowData(DataTable *t, const QVariantList &vals);
            SharedRowData(const SharedRowData &);

            SharedTableData *TableData() const;
            void SetTableData(SharedTableData *);

            QVariantCollection &Tuple() const;


        private:

            SharedTableData *_table_data;
            QVariantCollection *_tuple;

        };
    }
}

#endif // SHAREDDATAOBJECTS_H
