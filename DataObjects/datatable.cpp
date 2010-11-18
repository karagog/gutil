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
#include "datarowcollection.h"
#include "datacolumncollection.h"
#include "shareddataobjects.h"
#include "Utils/qstringhelpers.h"
#include "Core/exception.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QCoreApplication>
using namespace GUtil;

DataObjects::DataTable::DataTable(int num_cols)
    :QAbstractTableModel(qApp),
    _table_data(new SharedTableData)
{
    _init(QString::null, num_cols);
}

DataObjects::DataTable::DataTable(const QString &nm, int num_cols)
    :QAbstractTableModel(qApp),
    _table_data(new SharedTableData)
{
    _init(nm, num_cols);
}

DataObjects::DataTable::DataTable(const DataObjects::DataTable &o)
    :QAbstractTableModel(qApp)
{
    *this = o;
}

DataObjects::DataTable::DataTable(const DataObjects::DataSet &ds_parent)
    :QAbstractTableModel(qApp),
    _table_data(new SharedTableData(&ds_parent.set_data()))
{
    _init(QString::null, 0);
}

DataObjects::DataTable::DataTable(DataObjects::SharedTableData *td)
    :QAbstractTableModel(qApp),
    _table_data(td)
{}

DataObjects::DataTable::~DataTable(){}

void DataObjects::DataTable::_init(const QString &name, int num_cols)
{
    _table_data->name = name;

    QStringList sl;
    for(int i = 0; i < num_cols; i++)
        sl.append(QString("%1").arg(i));
    SetColumnHeaders(sl);
}

DataObjects::SharedTableData &DataObjects::DataTable::table_data() const
{
    return *_table_data;
}

DataObjects::DataRow &DataObjects::DataTable::operator [](int ind)
{
    return Rows()[ind];
}

const DataObjects::DataRow &DataObjects::DataTable::operator [](int ind) const
{
    return Rows()[ind];
}

DataObjects::DataTable &DataObjects::DataTable::operator =(const DataObjects::DataTable &o)
{
    _table_data = o._table_data;
    return *this;
}

bool DataObjects::DataTable::operator ==(const DataObjects::DataTable &o) const
{
    return _table_data == o._table_data;
}

bool DataObjects::DataTable::operator !=(const DataObjects::DataTable &o) const
{
    return !(*this == o);
}

bool DataObjects::DataTable::Equals(const DataObjects::DataTable &t) const
{
    bool ret = true;

    if(_table_data != t._table_data)
    {
        if(ColumnCount() == t.ColumnCount())
        {
            for(int i = 0; ret && i < ColumnCount(); i++)
            {
                ret = (_table_data->Columns().Key(i) == t._table_data->Columns().Key(i)) &&
                      (_table_data->Columns().Label(i) == t._table_data->Columns().Label(i));
            }

            if(ret)
            {
                if(RowCount() == t.RowCount())
                {
                    for(int i = 0; ret && i < RowCount(); i++)
                    {
                        ret = _table_data->Rows().At(i).Equals(
                                t._table_data->Rows()[i]);
                    }
                }
                else
                    ret = false;
            }
        }
        else
            ret = false;
    }

    return ret;
}

DataObjects::DataRowCollection &DataObjects::DataTable::Rows()
{
    return _table_data->Rows();
}

const DataObjects::DataRowCollection &DataObjects::DataTable::Rows() const
{
    return _table_data->Rows();
}

DataObjects::DataRow DataObjects::DataTable::AddRow(const DataObjects::DataRow &r)
{
    return Rows().Add(r);
}

DataObjects::DataRow DataObjects::DataTable::AddNewRow(const QVariantList &values)
{
    DataRow dr(CreateRow(values));
    AddRow(dr);
    return dr;
}

DataObjects::DataRow DataObjects::DataTable::CreateRow(const QVariantList &values)
{
    DataRow dr(*this);
    for(int i = 0; i < values.length() && i < ColumnCount(); i++)
        dr[i] = values.at(i);

    return dr;
}

DataObjects::DataRow DataObjects::DataTable::ImportRow(const DataObjects::DataRow &r)
{
    DataRow tmpr(r);
    r.CloneTo(tmpr);
    tmpr.row_data().SetTableData(_table_data.data());
    return AddRow(tmpr);
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
    table_data().Columns().Add( DataColumn(key, label) );

    for(int i = 0; i < RowCount(); i++)
        Rows()[i].set_number_of_columns(ColumnCount());
}

void DataObjects::DataTable::SetColumnHeaders(const QStringList &keys, const QStringList &labels)
{
    ClearColumns();

    foreach(QString k, keys)
        AddColumn(k);

    for(int i = 0; i < labels.length(); i++)
        table_data().Columns().SetLabel(i, labels.at(i));
}

void DataObjects::DataTable::SetColumnLabel(int col_index, const QString &l)
{
    table_data().Columns().Label(col_index) = l;
}

void DataObjects::DataTable::SetColumnKey(int col_index, const QString &k)
{
    table_data().Columns().SetKey(col_index, k);
}

void DataObjects::DataTable::ClearColumns()
{
    table_data().Columns().Clear();
}

int DataObjects::DataTable::RowCount() const
{
    return table_data().Rows().Count();
}

int DataObjects::DataTable::ColumnCount() const
{
    return table_data().Columns().Count();
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

DataObjects::DataSet DataObjects::DataTable::Set() const
{
    return DataSet(table_data().SetData());
}

QStringList DataObjects::DataTable::ColumnKeys() const
{
    return table_data().Columns().Keys();
}

QStringList DataObjects::DataTable::ColumnLabels() const
{
    return table_data().Columns().Labels();
}

QString DataObjects::DataTable::Name() const
{
    return _table_data->name;
}

void DataObjects::DataTable::SetTableName(const QString &n) const
{
    _table_data->name = n;
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
        table_data().Rows()[i].WriteXml(sw);

    write_xml_protected(sw);

    sw.writeEndElement();
}

#define READ_ERR_STRING "XML not in correct format"

void DataObjects::DataTable::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{
    FailIfReadOnly();

    Clear();

    if(sr.readNextStartElement())
    {
        if(sr.name() != DATATABLE_XML_ID)
            THROW_NEW_GUTIL_EXCEPTION( Core::XmlException, "Unrecognized XML Node" );

        int sz = sr.attributes().at(0).value().toString().toInt();

        _table_data->name = Utils::QStringHelpers::fromBase64(
                sr.attributes().at(1).value().toString());

        {
            QStringList new_keys = DataObjects::QVariantHelpers::ReadXml(sr).toStringList();
            QStringList new_labels = DataObjects::QVariantHelpers::ReadXml(sr).toStringList();
            SetColumnHeaders(new_keys, new_labels);
        }

        for(int i = 0; i < sz; i++)
        {
            DataObjects::DataRow dr = AddNewRow();
            dr.ReadXml(sr);
        }

        read_xml_protected(sr);

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
    t._table_data.detach();
    return t;
}

DataObjects::DataTable DataObjects::DataTable::Clone() const
{
    DataTable t(*this);
    return CloneTo(t);
}

void DataObjects::DataTable::commit_reject_changes(bool commit)
{
    if(commit)
    {

    }
    else
    {

    }
}
