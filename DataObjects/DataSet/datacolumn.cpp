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

#include "datacolumn.h"
#include "DataObjects/collection.h"
#include <QUuid>
using namespace GUtil;

DataObjects::DataColumn::DataColumn(const QString &key, const QString &label,
                                    DataObjects::DataColumnCollection *c)
    :_data(key == QString::null ? QUuid::createUuid().toString() : key,
           label),
    _collection(c)
{}

DataObjects::DataColumn::DataColumn(const DataObjects::DataColumn &o)
    :_data(o._data),
    _collection(o._collection)
{}

QString DataObjects::DataColumn::Key() const
{
    return _data.first;
}

QString DataObjects::DataColumn::Label() const
{
    return _data.second;
}

DataObjects::DataColumn &DataObjects::DataColumn::operator =(const DataObjects::DataColumn &o)
{
    _data = o._data;
    _collection = o._collection;
    return *this;
}

bool DataObjects::DataColumn::Equals(const DataObjects::DataColumn &o) const
{
    return _data.first == o._data.first;
}

void DataObjects::DataColumn::set_key(const QString &k)
{
    _data.first = k;
}

void DataObjects::DataColumn::set_label(const QString &l)
{
    _data.second = l;
}

int DataObjects::DataColumn::Index() const
{
    return ((Collection<DataColumn> *)_collection)->IndexOf(*this);
}

bool DataObjects::DataColumn::operator == (const DataObjects::DataColumn &o) const
{
    return Key() == o.Key();
}
bool DataObjects::DataColumn::operator != (const DataObjects::DataColumn &o) const
{
    return Key() != o.Key();
}
