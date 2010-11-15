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

#include "qvariantcollection.h"
#include <QStringList>
#include <QSharedData>

namespace GUtil
{
    namespace DataObjects
    {
        class DataSet;
        class DataTable;
        class DataTableCollection;
        class DataRow;
        class DataRowCollection;

        class SharedSetData : public QSharedData
        {
        public:
            SharedSetData(DataSet *);
            SharedSetData(const SharedSetData &);
            ~SharedSetData();

            DataTableCollection *tables;
        };



        class SharedTableData : public QSharedData
        {
        public:
            SharedTableData(DataTable *parent_table);
            SharedTableData(const SharedTableData &);
            ~SharedTableData();

            DataSet *dataset;
            DataRowCollection *rows;
            QStringList keys;
            QStringList labels;
            QString name;
        };



        class SharedRowData : public QSharedData
        {
        public:
            SharedRowData(DataTable *t, const QVariantList &vals);
            SharedRowData(const SharedRowData &);

            DataTable *Table() const;
            void SetTable(DataTable *);

            QVariantCollection tuple;

        private:
            DataTable *_table;
        };
    }
}

#endif // SHAREDDATAOBJECTS_H
