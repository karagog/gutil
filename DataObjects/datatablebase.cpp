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
#include "datarow.h"
#include "datarowcollectionbase.h"
#include "datacolumncollection.h"
#include "Custom/gvariant.h"
#include "Utils/qstringhelpers.h"
#include "Core/exception.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QCoreApplication>
using namespace GUtil;
using namespace Custom;
using namespace DataObjects;

template <class RowType> DataObjects::DataTableBase<RowType>::DataTableBase(int num_cols)
    :_table_data(new SharedTableData<RowType>)
{
    _init(QString::null, num_cols);
}

template <class RowType> DataObjects::DataTableBase<RowType>::
        DataTableBase(const QString &nm, int num_cols)
    :_table_data(new SharedTableData<RowType>)
{
    _init(nm, num_cols);
}

template <class RowType> DataObjects::DataTableBase<RowType>::
        DataTableBase(const DataObjects::DataTableBase<RowType> &o)
{
    *this = o;
}

template <class RowType> DataObjects::DataTableBase<RowType>::
        DataTableBase(const DataObjects::DataSet &ds_parent,
                      const QString &name,
                      int num_cols)
    :_table_data(new SharedTableData<RowType>(ds_parent))
{
    _init(name, num_cols);
}

template <class RowType> DataObjects::DataTableBase<RowType>::
        DataTableBase(DataObjects::SharedTableData<RowType> *td)
    :_table_data(td)
{}

template <class RowType> void DataObjects::DataTableBase<RowType>::_init(const QString &name, int num_cols)
{
    _table_data->_name = name;

    QStringList sl;
    for(int i = 0; i < num_cols; i++)
        sl.append(QString("%1").arg(i));
    SetColumnHeaders(sl);
}

template <class RowType> DataObjects::SharedTableData<RowType> &
        DataObjects::DataTableBase<RowType>::table_data() const
{
    return *_table_data;
}

template <class RowType> RowType &DataObjects::DataTableBase<RowType>::operator [](int ind)
{
    return Rows()[ind];
}

template <class RowType> const RowType &DataObjects::DataTableBase<RowType>::operator [](int ind) const
{
    return Rows()[ind];
}

template <class RowType> DataObjects::DataTableBase<RowType> &DataObjects::DataTableBase<RowType>::operator =(const DataObjects::DataTableBase<RowType> &o)
{
    _table_data = o._table_data;
    return *this;
}

template <class RowType> bool DataObjects::DataTableBase<RowType>::operator ==(const DataObjects::DataTableBase<RowType> &o) const
{
    return _table_data == o._table_data;
}

template <class RowType> bool DataObjects::DataTableBase<RowType>::operator !=(const DataObjects::DataTableBase<RowType> &o) const
{
    return !(*this == o);
}

template <class RowType> bool DataObjects::DataTableBase<RowType>::Equals(const DataObjects::DataTableBase<RowType> &t) const
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

template <class RowType> DataObjects::DataRowCollectionBase<RowType> &
        DataObjects::DataTableBase<RowType>::Rows()
{
    return _table_data->Rows();
}

template <class RowType> const DataObjects::DataRowCollectionBase<RowType> &
        DataObjects::DataTableBase<RowType>::Rows() const
{
    return _table_data->Rows();
}

template <class RowType> RowType &DataObjects::DataTableBase<RowType>::AddNewRow(const Custom::GVariantList &values)
{
    RowType dr(*this);

    for(int i = 0; i < values.length() && i < ColumnCount(); i++)
        dr[i] = values.at(i);

    return AddRow(dr);
}

template <class RowType> RowType &DataObjects::DataTableBase<RowType>::AddRow(const RowType &r)
{
    return Rows().Add(r);
}

template <class RowType> RowType &DataObjects::DataTableBase<RowType>::ImportRow(const RowType &r)
{
    RowType tmpr(r);
    r.CloneTo(tmpr);
    tmpr.row_data().Table() = *this;
    return AddRow(tmpr);
}

template <class RowType> void DataObjects::DataTableBase<RowType>::RemoveRow(int row_index)
{
    Rows().Remove(row_index);
}

template <class RowType> void DataObjects::DataTableBase<RowType>::RemoveRow(const RowType &r)
{
    Rows().RemoveOne(r);
}

template <class RowType> void DataObjects::DataTableBase<RowType>::Clear()
{
    Rows().Clear();

    ClearColumns();
}

template <class RowType> void DataObjects::DataTableBase<RowType>::AddColumn(const QString &key, const QString &label)
{
    table_data().Columns().Add( DataColumn(key, label) );

    for(int i = 0; i < RowCount(); i++)
        Rows()[i].set_number_of_columns(ColumnCount());
}

template <class RowType> void DataObjects::DataTableBase<RowType>::SetColumnHeaders(const QStringList &keys, const QStringList &labels)
{
    ClearColumns();

    foreach(QString k, keys)
        AddColumn(k);

    for(int i = 0; i < labels.length(); i++)
        table_data().Columns().SetLabel(i, labels.at(i));
}

template <class RowType> void DataObjects::DataTableBase<RowType>::SetColumnLabel(int col_index, const QString &l)
{
    table_data().Columns().Label(col_index) = l;
}

template <class RowType> void DataObjects::DataTableBase<RowType>::SetColumnKey(int col_index, const QString &k)
{
    table_data().Columns().SetKey(col_index, k);
}

template <class RowType> void DataObjects::DataTableBase<RowType>::ClearColumns()
{
    table_data().Columns().Clear();
}

template <class RowType> int DataObjects::DataTableBase<RowType>::RowCount() const
{
    return table_data().Rows().Count();
}

template <class RowType> int DataObjects::DataTableBase<RowType>::ColumnCount() const
{
    return table_data().Columns().Count();
}

template <class RowType> int DataObjects::DataTableBase<RowType>::GetColumnIndex(const QString &key) const
{
    int ret = -1;
    for(int i = 0; i < ColumnCount(); i++)
        if(ColumnKeys()[i] == key)
        {
            ret = i;
            break;
        }

    if(ret == -1)
        THROW_NEW_GUTIL_EXCEPTION(Core::IndexOutOfRangeException,
                                  QString("The column key '%1' does not exist in the %2 table")
                                  .arg(key).arg(table_data().GetName()).toStdString());

    return ret;
}

template <class RowType> DataObjects::DataSet DataObjects::DataTableBase<RowType>::Set() const
{
    return DataSet(table_data().GetDataSet());
}

template <class RowType> QStringList DataObjects::DataTableBase<RowType>::ColumnKeys() const
{
    return table_data().Columns().Keys();
}

template <class RowType> QStringList DataObjects::DataTableBase<RowType>::ColumnLabels() const
{
    return table_data().Columns().Labels();
}

template <class RowType> QString DataObjects::DataTableBase<RowType>::Name() const
{
    return _table_data->_name;
}

template <class RowType> void DataObjects::DataTableBase<RowType>::SetTableName(const QString &n) const
{
    _table_data->_name = n;
}






#define DATATABLE_XML_ID  "DataTable"

template <class RowType> void DataObjects::DataTableBase<RowType>::WriteXml(QXmlStreamWriter &sw) const
{
    sw.writeStartElement(DATATABLE_XML_ID);
    sw.writeAttribute("s", QString("%1").arg(RowCount()));
    sw.writeAttribute("n", QString("%1").arg(
            Utils::QStringHelpers::toBase64(Name())
            ));

    // Write our column data
    Custom::GVariant(ColumnKeys()).WriteXml(sw);
    Custom::GVariant(ColumnLabels()).WriteXml(sw);

    for(int i = 0; i < RowCount(); i++)
        table_data().Rows()[i].WriteXml(sw);

    write_xml_protected(sw);

    sw.writeEndElement();
}

#define READ_ERR_STRING "XML not in correct format"

template <class RowType> void DataObjects::DataTableBase<RowType>::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{
    FailIfReadOnly();

    Clear();

    if(sr.readNextStartElement())
    {
        if(sr.name() != DATATABLE_XML_ID)
            THROW_NEW_GUTIL_EXCEPTION( Core::XmlException, "Unrecognized XML Node" );

        int sz = sr.attributes().at(0).value().toString().toInt();

        _table_data->_name = Utils::QStringHelpers::fromBase64(
                sr.attributes().at(1).value().toString());

        {
            QStringList new_keys = GVariant::FromXml(sr).toStringList();
            QStringList new_labels = GVariant::FromXml(sr).toStringList();
            SetColumnHeaders(new_keys, new_labels);
        }

        for(int i = 0; i < sz; i++)
        {
            AddNewRow();
            (*this)[RowCount() - 1].ReadXml(sr);
        }

        read_xml_protected(sr);

        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != DATATABLE_XML_ID);
    }
}

template <class RowType> DataObjects::DataTableBase<RowType> &DataObjects::DataTableBase<RowType>::CloneTo(DataObjects::DataTableBase<RowType> &t) const
{
    t = *this;
    t._table_data.detach();
    return t;
}

template <class RowType> DataObjects::DataTableBase<RowType> DataObjects::DataTableBase<RowType>::Clone() const
{
    DataTableBase<RowType> t(*this);
    return CloneTo(t);
}

template <class RowType> void DataObjects::DataTableBase<RowType>::commit_reject_changes(bool commit)
{
    if(commit)
    {

    }
    else
    {

    }
}
