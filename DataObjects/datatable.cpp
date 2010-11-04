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

#include "datatable.h"
#include "Core/Utils/stringhelpers.h"
#include "Core/exception.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QCoreApplication>
using namespace GUtil;

DataObjects::DataTable::DataTable(int table_index, DataObjects::DataSet *ds_parent)
    :
    QAbstractTableModel(qApp),
    Core::Interfaces::IReadOnlyObject(false)
{
    _dataset = ds_parent;
    _index = table_index;
}











int DataObjects::DataTable::rowCount(const QModelIndex &) const
{
    return _dataset->get_table_values(_index).length();
}

int DataObjects::DataTable::columnCount(const QModelIndex &) const
{
    return _dataset->get_column_keys(_index).length();
}

QVariant DataObjects::DataTable::data(const QModelIndex &index, int role) const
{
    QVariant ret;


    return ret;
}

bool DataObjects::DataTable::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(IsReadOnly())
        return false;
}





void DataObjects::DataTable::WriteXml(QXmlStreamWriter &sw) const
{

}

#define READ_ERR_STRING "XML not in correct format"

void DataObjects::DataTable::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{

}


void DataObjects::DataTable::SetReadOnly(bool readonly)
{
    Core::Interfaces::IReadOnlyObject::SetReadOnly(readonly);

    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}
