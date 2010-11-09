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
    :QAbstractTableModel(qApp),
    table_data(new TableData())
{
    _init(0, num_cols);
}

DataObjects::DataTable::DataTable(const DataObjects::DataTable &o)
    :QAbstractTableModel(qApp)
{
    *this = o;
}

DataObjects::DataTable::DataTable(DataObjects::DataSet *ds_parent, int num_cols)
    :QAbstractTableModel(ds_parent),
    table_data(new TableData())
{
    _init(ds_parent, num_cols);
}

DataObjects::DataTable::~DataTable(){}

void DataObjects::DataTable::_init(DataSet *ds, int num_cols)
{
    table_data->dataset = ds;

    QStringList sl;
    for(int i = 0; i < num_cols; i++)
        sl.append(QString("%1").arg(i));
    SetColumnHeaders(sl);
}

DataObjects::DataTable &DataObjects::DataTable::operator =(const DataObjects::DataTable &o)
{
    table_data = o.table_data;
    return *this;
}

bool DataObjects::DataTable::operator ==(const DataObjects::DataTable &o) const
{
    return table_data == o.table_data;
}

DataObjects::DataRowCollection &DataObjects::DataTable::Rows()
{
    return table_data->rows;
}

DataObjects::DataRow DataObjects::DataTable::AddRow(const DataObjects::DataRow &r)
{
    Rows().Add(r);
    return Rows()[Rows().Count() - 1];
}

DataObjects::DataRow DataObjects::DataTable::AddNewRow(const QVariantList &values)
{
    DataObjects::DataRow dr = CreateRow(values);
    AddRow(dr);
    return dr;
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
    Rows().Remove(row_index);
}

void DataObjects::DataTable::Clear()
{
    Rows().ClearValues();

    ClearColumns();
}

void DataObjects::DataTable::AddColumn(const QString &key, const QString &label)
{
    if(ColumnKeys().contains(key))
        THROW_NEW_GUTIL_EXCEPTION( Core::Exception, "Key already exists in columns" );

    table_data->keys.append(key);
    table_data->labels.append(label);

    for(int i = 0; i < Rows().Count(); i++)
        Rows()[i].set_number_of_columns(ColumnCount());
}

void DataObjects::DataTable::SetColumnHeaders(const QStringList &keys, const QStringList &labels)
{
    ClearColumns();

    foreach(QString k, keys)
        AddColumn(k);

    for(int i = 0; i < labels.length(); i++)
        table_data->labels[i] = labels.at(i);
}

void DataObjects::DataTable::SetColumnLabel(int col_index, const QString &l)
{
    table_data->labels[col_index] = l;
}

void DataObjects::DataTable::SetColumnLabels(const QStringList &l)
{
    if(l.length() != ColumnCount())
        THROW_NEW_GUTIL_EXCEPTION( Core::Exception, "Incorrect number of columns" );

    table_data->labels = l;
}

void DataObjects::DataTable::SetColumnKey(int col_index, const QString &k)
{
    if(ColumnKeys().contains(k))
        THROW_NEW_GUTIL_EXCEPTION( Core::Exception, "Key already exists in columns" );

    table_data->keys[col_index] = k;
}

void DataObjects::DataTable::SetColumnKeys(const QStringList &k)
{
    ClearColumns();

    foreach(QString s, k)
        AddColumn(s);
}

void DataObjects::DataTable::ClearColumns()
{
    table_data->keys.clear();
    table_data->labels.clear();
}

int DataObjects::DataTable::RowCount() const
{
    return table_data->rows.Count();
}

int DataObjects::DataTable::ColumnCount() const
{
    return table_data->keys.count();
}

int DataObjects::DataTable::GetColumnIndex(const QString &key) const
{
    int ret = -1;
    for(int i = 0; i < ColumnCount(); i++)
        if(ColumnKeys()[i] == key)
        {
            ret = i;
            break;
        }

    return ret;
}

DataObjects::DataSet *DataObjects::DataTable::DataSetParent() const
{
    return table_data->dataset;
}

QStringList DataObjects::DataTable::ColumnKeys() const
{
    return table_data->keys;
}

QStringList DataObjects::DataTable::ColumnLabels() const
{
    return table_data->labels;
}







int DataObjects::DataTable::rowCount(const QModelIndex &) const
{
    return RowCount();
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




#define DATATABLE_XML_ID  "DataTable"

void DataObjects::DataTable::WriteXml(QXmlStreamWriter &sw) const
{
    sw.writeStartElement(DATATABLE_XML_ID);
    sw.writeAttribute("s", QString("%1").arg(RowCount()));

    // Write our column data
    DataObjects::QVariantHelpers::WriteXml(ColumnKeys(), sw);
    DataObjects::QVariantHelpers::WriteXml(ColumnLabels(), sw);

    for(int i = 0; i < RowCount(); i++)
        table_data->rows.Value(i).WriteXml(sw);

    sw.writeEndElement();
}

#define READ_ERR_STRING "XML not in correct format"

void DataObjects::DataTable::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{
    Clear();

    if(sr.readNextStartElement())
    {
        if(sr.name() != DATATABLE_XML_ID)
            THROW_NEW_GUTIL_EXCEPTION( Core::XmlException, "Unrecognized XML Node" );

        int sz = sr.attributes().at(0).value().toString().toInt();

        table_data->keys = DataObjects::QVariantHelpers::ReadXml(sr).toStringList();
        table_data->labels = DataObjects::QVariantHelpers::ReadXml(sr).toStringList();

        for(int i = 0; i < sz; i++)
        {
            DataObjects::DataRow dr = AddNewRow();
            dr.ReadXml(sr);
        }

        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != DATATABLE_XML_ID);
    }
}


void DataObjects::DataTable::SetReadOnly(bool readonly)
{
    Core::Interfaces::IReadOnlyObject::SetReadOnly(readonly);

    emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}







DataObjects::DataTable::TableData::TableData()
    :QSharedData(){}

DataObjects::DataTable::TableData::TableData(
        const DataObjects::DataTable::TableData &d)
            :QSharedData(d)
{
    dataset = d.dataset;
    rows = d.rows;
    keys = d.keys;
    labels = d.labels;
}











DataObjects::DataTableCollection::DataTableCollection(DataObjects::DataSet *ds)
{
    _ds = ds;
}

void DataObjects::DataTableCollection::onAdd(DataObjects::DataTable *tbl)
{
    tbl->table_data->dataset = _ds;
}
