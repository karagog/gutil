/*Copyright Copyright 2011 George Karagoulis

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
#include "sharedtabledata.h"
#include "dataset.h"
#include "datarow.h"
#include "datarowcollection.h"
#include "datacolumncollection.h"
#include "Custom/gvariant.h"
#include "Core/extendedexception.h"
#include "Core/gutil_globals.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QCoreApplication>
using namespace GUtil;
using namespace Custom;
using namespace DataObjects;
GUTIL_USING_CORE_NAMESPACE(DataObjects);

DataObjects::DataTable::DataTable(const QStringList &column_keys, const QStringList &column_labels)
        :ExplicitlySharedObject< SharedTableData >(new SharedTableData)
{
    _init(QString::null, 0);

    SetColumnHeaders(column_keys, column_labels);
}

DataObjects::DataTable::DataTable(const QString &nm, int num_cols)
    :ExplicitlySharedObject< SharedTableData >(new SharedTableData)
{
    _init(nm, num_cols);
}

DataObjects::DataTable::DataTable(int num_cols)
    :ExplicitlySharedObject< SharedTableData >(new SharedTableData)
{
    _init(QString::null, num_cols);
}

DataObjects::DataTable::DataTable(const DataObjects::DataTable &o)
    :ExplicitlySharedObject< SharedTableData >(o),
      Interfaces::IQXmlSerializable(),
      Core::Interfaces::IReadOnlyObject(),
      Core::Interfaces::IUpdatable(),
      Core::Interfaces::IClonable<DataTable>(),
      Core::Interfaces::IEquatable<DataTable>()
{}

DataObjects::DataTable::DataTable(DataObjects::SharedTableData *td)
    :ExplicitlySharedObject< SharedTableData >(td)
{}

void DataObjects::DataTable::_init(const QString &name, int num_cols)
{
    table_data().Name = name;

    Columns().Resize(num_cols);
}

DataObjects::TableData &DataObjects::DataTable::table_data()
{
    return *ExplicitlySharedObject<SharedTableData>::GetExplicitlySharedData();
}

const TableData &DataObjects::DataTable::table_data() const
{
    return *ExplicitlySharedObject< SharedTableData >::GetExplicitlySharedData();
}

DataObjects::DataTable &DataObjects::DataTable::operator =(const DataObjects::DataTable &o)
{
    SetExplicitlySharedData(o.GetExplicitlySharedData());
    return *this;
}

bool DataObjects::DataTable::Equals(const DataObjects::DataTable &t) const
{
    bool ret = true;

    if(*this != t)
    {
        if(ColumnCount() == t.ColumnCount())
        {
            for(int i = 0; ret && i < ColumnCount(); i++)
            {
                ret = (table_data().Columns[i].GetKey() == t.table_data().Columns[i].GetKey()) &&
                      (table_data().Columns[i].GetLabel() == t.table_data().Columns[i].GetLabel());
            }

            if(ret)
            {
                if(RowCount() == t.RowCount())
                {
                    for(int i = 0; ret && i < RowCount(); i++)
                    {
                        ret = table_data().Rows.At(i).Equals(
                                t.table_data().Rows[i]);
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

DataRow &DataObjects::DataTable::add_new_row(const Custom::GVariantVector &values)
{
    return add_row(create_row(values));
}

DataRow &DataTable::add_row(const DataRow &r)
{
    DataRow &ret = Rows().Add(r);

    if(_key_violations())
    {
        Rows().Remove(Rows().Count() - 1);

        THROW_NEW_GUTIL_EXCEPTION2(Core::ValidationException,
                                  "The primary key would be violated");
    }

    return ret;
}

DataRow DataTable::create_row(const Custom::GVariantVector &values)
{
    DataRow dr(&table_data(), values);
    init_new_row(dr);
    return dr;
}

DataObjects::DataRow &DataObjects::DataTable::import_row(const DataObjects::DataRow &r)
{
    DataRow tmpr(r);
    r.CloneTo(tmpr);
    tmpr.row_data().Table = &table_data();
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

    Columns().Clear();
}

void DataObjects::DataTable::SetColumnHeaders(const QStringList &keys, const QStringList &labels)
{
    int new_col_count( gMax(keys.count(), labels.count()) );
    Columns().Resize(new_col_count);

    for(int i = 0; i < new_col_count; i++)
    {
        DataColumn newcol(i < keys.count() ? keys[i] : QUuid::createUuid().toString(),
                          i < labels.count() ? labels[i] : "");
        Columns().validate_new_item(newcol);
        Columns()[i] = newcol;
    }
}

QStringList DataObjects::DataTable::ColumnKeys() const
{
    return table_data().Columns.Keys();
}

QStringList DataObjects::DataTable::ColumnLabels() const
{
    return table_data().Columns.Labels();
}

void DataObjects::DataTable::SetTableName(const QString &n)
{
    table_data().Name = n;
}






#define DATATABLE_XML_ID  "DataTable"

void DataObjects::DataTable::WriteXml(QXmlStreamWriter &sw) const
{
    sw.writeStartElement(DATATABLE_XML_ID);
    sw.writeAttribute("s", QString("%1").arg(RowCount()));
    sw.writeAttribute("n", QString("%1").arg(
            String(Name()).ToBase64()
            ));

    // Write our column data
    Custom::GVariant(ColumnKeys()).WriteXml(sw);
    Custom::GVariant(ColumnLabels()).WriteXml(sw);

    for(int i = 0; i < RowCount(); i++)
        table_data().Rows[i].WriteXml(sw);

    sw.writeEndElement();
}

#define READ_ERR_STRING "XML not in correct format"

void DataObjects::DataTable::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException<true>)
{
    FailIfReadOnly();

    Clear();

    if(sr.readNextStartElement())
    {
        if(sr.name() != DATATABLE_XML_ID){
            Core::XmlException<true> ex("Unrecognized XML Node");
            ex.SetData("elt_name", sr.name().toString().toAscii().constData());
            THROW_GUTIL_EXCEPTION(ex);
        }

        int sz = sr.attributes().at(0).value().toString().toInt();

        table_data().Name = String(sr.attributes().at(1).value().toString()).FromBase64().ToQString();

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

        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != DATATABLE_XML_ID);
    }
    else
    {
        Core::XmlException<true> ex("Not a valid DataTable serialization");
        ex.SetData("Token", sr.tokenString().toAscii().constData());
        THROW_GUTIL_EXCEPTION(ex);
    }
}

DataObjects::DataTable &DataObjects::DataTable::CloneTo(DataObjects::DataTable &t) const
{
    t = *this;
    t.Detach();
    return t;
}

DataObjects::DataTable DataObjects::DataTable::Clone() const
{
    DataTable t(*this);
    return CloneTo(t);
}

void DataObjects::DataTable::commit_reject_changes(bool commit)
{
    table_data().CommitChanges(commit);
}

QSet<int> DataObjects::DataTable::KeyColumns() const
{
    return table_data().KeyColumns;
}

void DataObjects::DataTable::AddKeyColumn(int k)
{
    if(0 <= k && k < ColumnCount())
    {
        table_data().KeyColumns.insert(k);

        if(_key_violations())
        {
            table_data().KeyColumns.remove(k);

            THROW_NEW_GUTIL_EXCEPTION2(Core::ValidationException,
                                      "The table's primary key would be violated");
        }
    }
}

void DataObjects::DataTable::AddKeyColumn(const QString &k)
{
    AddKeyColumn(GetColumnIndex(k));
}

void DataObjects::DataTable::RemoveKeyColumn(int k)
{
    if(0 <= k && k < ColumnCount())
    {
        table_data().KeyColumns.remove(k);

        if(_key_violations())
        {
            table_data().KeyColumns.insert(k);

            THROW_NEW_GUTIL_EXCEPTION2(Core::ValidationException,
                                      "The table's primary key would be violated");
        }
    }
}

void DataObjects::DataTable::RemoveKeyColumn(const QString &k)
{
    RemoveKeyColumn(GetColumnIndex(k));
}

void DataObjects::DataTable::on_make_dirty()
{
    table_data().MakeDirty();
}

bool DataObjects::DataTable::_key_violations() const
{
    if(KeyColumns().count() == 0)
        return false;

    bool key_violation = false;
    for(int i = 0; !key_violation && i < RowCount(); i++)
    {
        const DataRow &cur_row = Rows()[i];

        for(int j = i + 1; !key_violation && j < RowCount(); j++)
        {
            const DataRow &cmp_row = Rows()[j];

            key_violation = true;
            foreach(int k, KeyColumns())
            {
                key_violation = key_violation &&
                                (cmp_row[k] == cur_row[k]);

                if(!key_violation)
                    break;
            }
        }
    }

    return key_violation;
}

void DataObjects::DataTable::LockForRead()
{
    table_data().Lock.lockForRead();
}

void DataObjects::DataTable::LockForWrite()
{
    table_data().Lock.lockForWrite();
}

bool DataObjects::DataTable::TryLockForRead()
{
    return table_data().Lock.tryLockForRead();
}

bool DataObjects::DataTable::TryLockForWrite()
{
    return table_data().Lock.tryLockForWrite();
}

void DataObjects::DataTable::Unlock()
{
    table_data().Lock.unlock();
}

DataRow &DataObjects::DataTable::FindFirstRow(const QMap<int, GVariant> &keycolumn_value_mapping)
        throw(Core::NotFoundException<true>)
{
    if(keycolumn_value_mapping.count() == 0)
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotFoundException,
                                  "You didn't provide any keys by which to search");

    for(int i = 0; i < RowCount(); i++)
    {
        bool found = true;
        foreach(int c, keycolumn_value_mapping.keys())
        {
            if(!found)
                break;

            if(c < 0 || c >= ColumnCount())
            {
                found = false;
                break;
            }

            found = found && keycolumn_value_mapping[c] == At(i)[c];
        }

        if(found)
            return At(i);
    }

    Core::NotFoundException<true> ex("The row was not found.  Check exception data for search criteria.");
    int cnt = 1;
    foreach(int c, keycolumn_value_mapping.keys())
        ex.SetData(QString("k%1").arg(cnt++).toAscii().constData(),
                   QString("Column %1: %2").arg(c).arg(keycolumn_value_mapping[c].toString())
                   .toAscii().constData());

    THROW_GUTIL_EXCEPTION(ex);
}

const DataRow &DataObjects::DataTable::FindFirstRow(const QMap<int, GVariant> &keycolumn_value_mapping) const
        throw(Core::NotFoundException<true>)
{
    if(keycolumn_value_mapping.count() == 0)
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotFoundException,
                                  "You didn't provide any keys by which to search");

    for(int i = 0; i < RowCount(); i++)
    {
        bool found = true;
        foreach(int c, keycolumn_value_mapping.keys())
        {
            if(!found)
                break;

            if(c < 0 || c >= ColumnCount())
            {
                found = false;
                break;
            }

            found = found && keycolumn_value_mapping[c] == At(i)[c];
        }

        if(found)
            return At(i);
    }

    Core::NotFoundException<true> ex("The row was not found.  Check exception data for search criteria.");
    int cnt = 1;
    foreach(int c, keycolumn_value_mapping.keys())
        ex.SetData(QString("k%1").arg(cnt++).toAscii().constData(),
                   QString("Column %1: %2").arg(c).arg(keycolumn_value_mapping[c].toString())
                   .toAscii().constData());

    THROW_GUTIL_EXCEPTION(ex);
}

DataRow &DataObjects::DataTable::
        FindFirstRow(const QMap<QString, GVariant> &keycolumn_value_mapping)
        throw(Core::NotFoundException<true>)
{
    QMap<int, Custom::GVariant> tmp;
    foreach(QString s, keycolumn_value_mapping.keys())
        tmp.insert(GetColumnIndex(s), keycolumn_value_mapping[s]);
    return FindFirstRow(tmp);
}

const DataRow &DataObjects::DataTable::
        FindFirstRow(const QMap<QString, GVariant> &keycolumn_value_mapping) const
        throw(Core::NotFoundException<true>)
{
    QMap<int, Custom::GVariant> tmp;
    foreach(QString s, keycolumn_value_mapping.keys())
        tmp.insert(GetColumnIndex(s), keycolumn_value_mapping[s]);
    return FindFirstRow(tmp);
}

DataRow &DataObjects::DataTable::FindFirstRow(int col, const Custom::GVariant &v)
{
    QMap<int, Custom::GVariant> m;
    m.insert(col, v);
    return FindFirstRow(m);
}

const DataRow &DataObjects::DataTable::FindFirstRow(int col, const Custom::GVariant &v) const
{
    QMap<int, Custom::GVariant> m;
    m.insert(col, v);
    return FindFirstRow(m);
}

DataRow &DataObjects::DataTable::FindFirstRow(const QString &col, const Custom::GVariant &v)
{
    QMap<int, Custom::GVariant> m;
    m.insert(GetColumnIndex(col), v);
    return FindFirstRow(m);
}

const DataRow &DataObjects::DataTable::FindFirstRow(const QString &col, const Custom::GVariant &v) const
{
    QMap<int, Custom::GVariant> m;
    m.insert(GetColumnIndex(col), v);
    return FindFirstRow(m);
}



DataRowCollection DataObjects::DataTable::
        FindRows(const QMap<int, GVariant> &keycolumn_value_mapping) const
{
    if(keycolumn_value_mapping.count() == 0)
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotFoundException,
                                  "You didn't provide any keys by which to search");

    DataRowCollection ret(GetExplicitlySharedData());

    for(int i = 0; i < RowCount(); i++)
    {
        bool found = true;
        foreach(int c, keycolumn_value_mapping.keys())
        {
            if(!found)
                break;

            if(c < 0 || c >= ColumnCount())
            {
                found = false;
                break;
            }

            found = found && keycolumn_value_mapping[c] == At(i)[c];
        }

        if(found)
            ret.Add(At(i));
    }

    return ret;
}

DataRowCollection DataObjects::DataTable::
        FindRows(const QMap<QString, GVariant> &keycolumn_value_mapping) const
{
    QMap<int, Custom::GVariant> tmp;
    foreach(QString s, keycolumn_value_mapping.keys())
        tmp.insert(GetColumnIndex(s), keycolumn_value_mapping[s]);
    return FindRows(tmp);
}

DataRowCollection DataObjects::DataTable::FindRows(int col, const Custom::GVariant &v) const
{
    QMap<int, Custom::GVariant> m;
    m.insert(col, v);
    return FindRows(m);
}

DataRowCollection DataObjects::DataTable::
        FindRows(const QString &col, const Custom::GVariant &v) const
{
    QMap<int, Custom::GVariant> m;
    m.insert(GetColumnIndex(col), v);
    return FindRows(m);
}
