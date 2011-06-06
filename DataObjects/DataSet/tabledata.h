/*Copyright Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef TABLEDATA_H
#define TABLEDATA_H

#include "DataObjects/DataSet/datacolumncollection.h"
#include "DataObjects/DataSet/datarowcollection.h"
#include "Core/Interfaces/iupdatable.h"
#include <QSet>
#include <QReadWriteLock>

GUTIL_BEGIN_NAMESPACE( DataObjects );


class TableData :
        public Core::Interfaces::IUpdatable
{
public:

    TableData();
    TableData(const TableData &d);

    int GetColumnIndex(const QString &key) const;

    DataRowCollection Rows;
    DataColumnCollection Columns;
    QString Name;
    QSet<int> KeyColumns;

    QReadWriteLock Lock;


protected:

    void commit_reject_changes(bool commit);

};


GUTIL_END_NAMESPACE

#endif // TABLEDATA_H
