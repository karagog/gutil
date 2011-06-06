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

#include "datacolumncollection.h"
#include "datatable.h"
#include <QStringList>
using namespace GUtil;

DataObjects::DataColumnCollection::DataColumnCollection(SharedTableData *t, int size)
    :ResizableCollection<DataColumn>(size),
      _table(new DataTable(t))
{}

DataObjects::DataColumnCollection::~DataColumnCollection()
{
    delete _table;
}

DataObjects::DataColumnCollection::DataColumnCollection(const DataColumnCollection &o)
    :ResizableCollection<DataColumn>(o),
      _table(new DataTable(*o._table))
{}

bool DataObjects::DataColumnCollection::ContainsKey(const QString &k) const
{
    return Contains( DataColumn(k) );
}

QStringList DataObjects::DataColumnCollection::Keys() const
{
    QStringList ret;

    for(int i = 0; i < Count(); i++)
        ret.append(At(i).GetKey());

    return ret;
}

QStringList DataObjects::DataColumnCollection::Labels() const
{
    QStringList ret;

    for(int i = 0; i < Count(); i++)
        ret.append(At(i).GetLabel());

    return ret;
}

void DataObjects::DataColumnCollection::validate_new_item(const DataObjects::DataColumn &c) const
        throw(Core::ValidationException)
{
    if(Contains(c))
        THROW_NEW_GUTIL_EXCEPTION2(Core::ValidationException,
                                  QString("Column collection already contains key '%1'")
                                  .arg(c.GetKey()).toStdString());
}

void DataObjects::DataColumnCollection::on_add(int i)
{
    _table->column_inserted(i);
}

void DataObjects::DataColumnCollection::on_remove(int i)
{
    _table->column_removed(i);
}
