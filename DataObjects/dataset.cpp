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
#include "dataset.h"
#include <QCoreApplication>
#include <QVariantList>
using namespace GUtil;

DataObjects::DataSet::DataSet(QObject *parent)
    :QAbstractItemModel(parent == 0 ? qApp : parent)
{
    _auto_commit = false;
    _dirty = false;

    _tables = new DataTableCollection(this);
}

DataObjects::DataSet::~DataSet()
{
    delete _tables;
}

DataObjects::DataTableCollection &DataObjects::DataSet::Tables()
{
    return *_tables;
}

void DataObjects::DataSet::set_value(int table_index,
                                     int row_index,
                                     int col_index,
                                     const QVariant &value)
{
    data[table_index][row_index][col_index] = value;
}

QVariant DataObjects::DataSet::get_value(int table_index, int row_index, int col_index) const
{
    return data.at(table_index).at(row_index).at(col_index);
}

void DataObjects::DataSet::set_row_values(int table_index, int row_index, const QVariantList &values)
{
    data[table_index][row_index] = values;
}

QVariantList DataObjects::DataSet::get_row_values(int table_index, int row_index) const
{
    return data.at(table_index).at(row_index);
}

void DataObjects::DataSet::set_table_values(int table_index, const QList<QVariantList> &values)
{
    data[table_index] = values;
}

QList<QVariantList> DataObjects::DataSet::get_table_values(int table_index) const
{
    return data[table_index];
}

QStringList DataObjects::DataSet::get_column_keys(int index) const
{
    //return _keys.at(index);
}









bool DataObjects::DataSet::Dirty() const
{
    return _dirty;
}

void DataObjects::DataSet::CommitChanges()
{
    commit_reject_changes(true);
}

void DataObjects::DataSet::RejectChanges()
{
    commit_reject_changes(false);
}

void DataObjects::DataSet::commit_reject_changes(bool commit)
{
    if(commit)
    {
        //_orig_data = data;
    }
    else
    {
        //data = _orig_data;
    }
}

void DataObjects::DataSet::SetAutoCommitChanges(bool val)
{
    _auto_commit = val;
}

bool DataObjects::DataSet::AutoCommitChanges() const
{
    return _auto_commit;
}



void DataObjects::DataSet::WriteXml(QXmlStreamWriter &sw) const
{

}

void DataObjects::DataSet::ReadXml(QXmlStreamReader &sr)
        throw(GUtil::Core::XmlException)
{

}
