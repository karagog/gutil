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
#include <QUuid>
using namespace GUtil;

DataObjects::DataColumnCollection::DataColumnCollection(int size)
    :Collection<DataColumn>(size)
{}

DataObjects::DataColumnCollection::DataColumnCollection(const DataColumnCollection &o)
    :Collection<DataColumn>(o)
{}

QString DataObjects::DataColumnCollection::Key(int ind) const
{
    return Value(ind).Key();
}

bool DataObjects::DataColumnCollection::ContainsKey(const QString &k) const
{
    return Contains(DataColumn(k));
}

QString DataObjects::DataColumnCollection::Label(int ind) const
{
    return Value(ind).Label();
}

DataColumn DataObjects::DataColumnCollection::create_blank_item() const
{
    return DataColumn(QUuid::createUuid().toString());
}

void DataObjects::DataColumnCollection::validate_new_item(const DataObjects::DataColumn &c) const
        throw(Core::ValidationException)
{
    if(Contains(c))
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  QString("Column collection already contains key '%1'")
                                  .arg(c.first).toStdString());
}

void DataObjects::DataColumnCollection::compare_equality(
        const DataObjects::DataColumn &lhs,
        const DataObjects::DataColumn &rhs) const
{
    return lhs.Equals(rhs);
}
