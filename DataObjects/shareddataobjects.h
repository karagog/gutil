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

            SharedSetData();
            SharedSetData(const SharedSetData &);
            virtual ~SharedSetData();

            DataTableCollection &Tables() const;


        protected:

            DataTableCollection *tables;

        };



        class SharedTableData : public QSharedData
        {
        public:

            SharedTableData(SharedSetData *sd = 0);
            SharedTableData(const SharedTableData &);
            virtual ~SharedTableData();

            SharedSetData *SetData() const;
            void SetSetData(SharedSetData *);

            DataRowCollection &Rows() const;
            DataColumnCollection &Columns() const;
            QString name;


        protected:

            SharedSetData *set_data;
            DataRowCollection *rows;
            DataColumnCollection *columns;

        };



        class SharedRowData : public QSharedData
        {
        public:

            SharedRowData(SharedTableData *, const QVariantList &vals);
            SharedRowData(const SharedRowData &);
            virtual ~SharedRowData();

            SharedTableData *TableData() const;
            void SetTableData(SharedTableData *);

            QVariantCollection &Tuple() const;


        protected:

            SharedTableData *table_data;
            QVariantCollection *tuple;

        };
    }
}

#endif // SHAREDDATAOBJECTS_H
