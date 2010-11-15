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
using namespace GUtil;

DataObjects::DataColumn::DataColumn(const QString &key)
{
    _data.first = key;
}

DataObjects::DataColumn::DataColumn(const QPair<QString, QString> &o)
    :QPair<QString, QString>(o){}

DataObjects::DataColumn::DataColumn(const DataObjects::DataColumn &o)
    :QPair<QString, QString>(o._data){}

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
}

bool DataObjects::DataColumn::Equals(const DataObjects::DataColumn &o) const
{
    return _data.first == o._data.first;
}
