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
#include "Utils/qstringhelpers.h"
#include "Core/exception.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QCoreApplication>
using namespace GUtil;

DataObjects::DataTable::DataTable(int num_cols)
    :QAbstractTableModel(qApp),
    table_data(new TableData(this))
{
    _init(0, QString::null, num_cols);
}

DataObjects::DataTable::DataTable(const QString &nm, int num_cols)
    :QAbstractTableModel(qApp),
    table_data(new TableData(this))
{
    _init(0, nm, num_cols);
}

DataObjects::DataTable::DataTable(const DataObjects::DataTable &o)
    :QAbstractTableModel(qApp)
{
    *this = o;
}

DataObjects::DataTable::DataTable(DataObjects::DataSet *ds_parent)
    :QAbstractTableModel(ds_parent),
    table_data(new TableData(this))
{
    _init(ds_parent, QString::null, 0);
}

DataObjects::DataTable::~DataTable(){}

void DataObjects::DataTable::_init(DataSet *ds, const QString &name, int num_cols)
{
    table_data->dataset = ds;
    table_data->name = name;

    QStringList sl;
    for(int i = 0; i < num_cols; i++)
        sl.append(QString("%1").arg(i));
    SetColumnHeaders(sl);
}

DataObjects::DataRow &DataObjects::DataTable::operator [](int ind)
{
    return Rows()[ind];
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
        throw(Core::ArgumentException)
{
    if(r.row_data->table != this)
        THROW_NEW_GUTIL_EXCEPTION(Core::ArgumentException,
                                  "The row does not belong to this table.  Maybe "
                                  "you meant 'ImportRow'?");
    else if(Rows().Contains(r))
        THROW_NEW_GUTIL_EXCEPTION(Core::ArgumentException,
                                  "This row already exists in the table");

    Rows().Add(r);
    return Rows().Value(Rows().Count() - 1);
}

DataObjects::DataRow DataObjects::DataTable::AddNewRow(const QVariantList &values)
{
    DataRow dr(CreateRow(values));
    AddRow(dr);
    return dr;
}

DataObjects::DataRow DataObjects::DataTable::CreateRow(const QVariantList &values)
{
    DataRow dr(this);
    for(int i = 0; i < values.length(); i++)
        dr[i] = values.at(i);

    return dr;
}

DataObjects::DataRow DataObjects::DataTable::ImportRow(const DataObjects::DataRow &r)
{
    DataRow tmpr;
    r.CloneTo(tmpr);
    tmpr.set_table(this);
    return this->AddRow(tmpr);
}

void DataObjects::DataTable::RemoveRow(int row_index)
{
    Rows().Remove(row_index);
}

void DataObjects::DataTable::RemoveRow(const DataRow &r)
{
    Rows().RemoveOne(r);
}

void DataObjects::DataTable::Clear()
{
    Rows().Clear();

    ClearColumns();
}

void DataObjects::DataTable::AddColumn(const QString &key, const QString &label)
{
    if(ColumnKeys().contains(key))
        THROW_NEW_GUTIL_EXCEPTION(Core::ArgumentException,
                                  "Key already exists in columns");

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
        THROW_NEW_GUTIL_EXCEPTION(Core::ArgumentException,
                                  "Incorrect number of columns");

    table_data->labels = l;
}

void DataObjects::DataTable::SetColumnKey(int col_index, const QString &k)
{
    if(ColumnKeys().contains(k))
        THROW_NEW_GUTIL_EXCEPTION(Core::ArgumentException,
                                  "Key already exists in columns");

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

QString DataObjects::DataTable::Name() const
{
    return table_data->name;
}

void DataObjects::DataTable::SetTableName(const QString &n) const
{
    table_data->name = n;
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
    sw.writeAttribute("n", QString("%1").arg(
            Utils::QStringHelpers::toBase64(Name())
            ));

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

        table_data->name = Utils::QStringHelpers::fromBase64(
                sr.attributes().at(1).value().toString());

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

DataObjects::DataTable &DataObjects::DataTable::CloneTo(DataObjects::DataTable &t) const
{
    t = *this;
    t.table_data.detach();
    return t;
}







DataObjects::DataTable::TableData::TableData(DataObjects::DataTable *t)
    :QSharedData(), rows(t)
{}

DataObjects::DataTable::TableData::TableData(
        const DataObjects::DataTable::TableData &d)
            :QSharedData(d),
            rows(d.rows),
            keys(d.keys),
            labels(d.labels),
            name(d.name)
{
    dataset = d.dataset;
}




DataObjects::DataTableCollection::DataTableCollection(DataObjects::DataSet *d)
{
    _dataset = d;
}

DataObjects::DataTableCollection::DataTableCollection(const DataTableCollection &o)
{
    o.CloneTo(*this);
}

DataObjects::DataTableCollection &DataObjects::DataTableCollection::CloneTo(
        DataTableCollection &o) const
{
    o.Resize(Size());

    for(int i = 0; i < Size(); i++)
        this->Value(i).CloneTo(o[i]);

    return o;
}

void DataObjects::DataTableCollection::on_add(DataObjects::DataTable &t) const
{
    t.table_data->dataset = _dataset;
}

DataObjects::DataTable DataObjects::DataTableCollection::create_blank_item() const
{
    return DataTable(_dataset);
}

