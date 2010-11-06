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
#include "dataset.h"
#include "datarow.h"
#include "Core/Utils/stringhelpers.h"
#include "Core/exception.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QCoreApplication>
using namespace GUtil;

DataObjects::DataTable::DataTable(int num_cols)
    :QAbstractTableModel(qApp)
{
    _init(0, num_cols);
}

DataObjects::DataTable::DataTable(const DataTable &o)
    :QAbstractTableModel(qApp)
{
    _init(0, 0);

    *this = o;
}

DataObjects::DataTable::DataTable(DataObjects::DataSet *ds_parent, int num_cols)
    :QAbstractTableModel(ds_parent)
{
    _init(ds_parent, num_cols);
}

DataObjects::DataTable::~DataTable(){}

void DataObjects::DataTable::_init(DataSet *ds, int num_cols)
{
    dataset = ds;

    _rows = new DataObjects::DataRowCollection(this);

    QStringList sl;
    for(int i = 0; i < num_cols; i++)
        sl.append(QString("%1").arg(i));
    SetColumnHeaders(sl);
}

DataObjects::DataTable &DataObjects::DataTable::operator =(const DataObjects::DataTable &o)
{
    this->dataset = o.dataset;

    _keys = o._keys;
    _labels = o._labels;

    *_rows = *o._rows;

    return *this;
}

DataObjects::DataRowCollection &DataObjects::DataTable::Rows()
{
    return *_rows;
}

DataObjects::DataRow DataObjects::DataTable::AddRow(const DataObjects::DataRow &r)
{
    _rows->Add(r);
    DataObjects::DataRow &mr = (*_rows)[_rows->Count() - 1];

    mr.set_number_of_columns(ColumnCount());
    return mr;
}

DataObjects::DataRow DataObjects::DataTable::CreateRow(const QVariantList &values)
{
    DataObjects::DataRow dr(this);
    for(int i = 0; i < values.length(); i++)
        dr[i] = values.at(i);

    return dr;
}

void DataObjects::DataTable::RemoveRow(int row_index)
{
    _rows->Remove(row_index);
}

void DataObjects::DataTable::AddColumn(const QString &key, const QString &label)
{
    if(_keys.contains(key))
    {
        THROW_GUTIL_EXCEPTION( Core::Exception("Key already exists in columns") )
    }

    _keys.append(key);
    _labels.append(label);
}

void DataObjects::DataTable::SetColumnHeaders(const QStringList &keys, const QStringList &labels)
{
    ClearColumns();

    foreach(QString k, keys)
        AddColumn(k);

    for(int i = 0; i < ColumnCount(); i++)
    {
        QString s;
        if(labels.length() != 0)
            s = labels.at(i);
        _labels.append(s);
    }
}

void DataObjects::DataTable::SetColumnLabel(int col_index, const QString &l)
{
    _labels[col_index] = l;
}

void DataObjects::DataTable::SetColumnKey(int col_index, const QString &k)
{
    _keys[col_index] = k;
}

void DataObjects::DataTable::ClearColumns()
{
    _keys.clear();
    _labels.clear();
}

int DataObjects::DataTable::ColumnCount() const
{
    return _keys.count();
}

int DataObjects::DataTable::GetColumnIndex(const QString &key) const
{
    int ret = -1;
    for(int i = 0; i < ColumnCount(); i++)
        if(_keys[i] == key)
        {
            ret = i;
            break;
        }

    return ret;
}







int DataObjects::DataTable::rowCount(const QModelIndex &) const
{
    return _rows->Count();
}

int DataObjects::DataTable::columnCount(const QModelIndex &) const
{
    return ColumnCount();
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

bool DataObjects::DataTable::insertRows(int row, int count, const QModelIndex &parent)
{

}

bool DataObjects::DataTable::removeRows(int row, int count, const QModelIndex &parent)
{

}

QVariant DataObjects::DataTable::headerData(
        int section,
        Qt::Orientation orientation,
        int role) const
{

}

Qt::ItemFlags DataObjects::DataTable::flags(const QModelIndex &index) const
{

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














DataObjects::DataTableCollection::DataTableCollection(DataObjects::DataSet *ds)
{
    _ds = ds;
}

void DataObjects::DataTableCollection::onAdd(void *tbl, int)
{
    DataTable *t = (DataTable *)tbl;
    t->dataset = _ds;
}
