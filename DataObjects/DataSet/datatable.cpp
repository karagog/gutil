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
#include "sharedtabledata.h"
#include "dataset.h"
#include "datarow.h"
#include "datarowcollection.h"
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

DataObjects::DataTable::DataTable(int num_cols)
    :ExplicitlySharedObject< SharedTableData >(new SharedTableData)
{
    _init(QString::null, num_cols);
}

DataObjects::DataTable::DataTable(const QString &nm, int num_cols)
    :ExplicitlySharedObject< SharedTableData >(new SharedTableData)
{
    _init(nm, num_cols);
}

DataObjects::DataTable::
        DataTable(const DataObjects::DataTable &o)
            :ExplicitlySharedObject< SharedTableData >(o){}

DataObjects::DataTable::
        DataTable(const DataObjects::DataSet &ds_parent,
                      const QString &name,
                      int num_cols)
    :ExplicitlySharedObject< SharedTableData >(new SharedTableData(ds_parent))
{
    _init(name, num_cols);
}

DataObjects::DataTable::DataTable(DataObjects::SharedTableData *td)
    :ExplicitlySharedObject< SharedTableData >(td){}

DataObjects::DataTable::~DataTable(){}

void DataObjects::DataTable::_init(const QString &name, int num_cols)
{
    table_data().SetName(name);

    QStringList sl;
    for(int i = 0; i < num_cols; i++)
        sl.append(QString("%1").arg(i));
    SetColumnHeaders(sl);
}

DataObjects::SharedTableData &
        DataObjects::DataTable::table_data()
{
    return *ExplicitlySharedObject<SharedTableData>::GetExplicitlySharedData();
}

const SharedTableData &
        DataObjects::DataTable::table_data() const
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

    // This operator compares their pointers (defined in ExplicitlySharedObject)
    if(*this != t)
    {
        if(ColumnCount() == t.ColumnCount())
        {
            for(int i = 0; ret && i < ColumnCount(); i++)
            {
                ret = (table_data().Columns().Key(i) == t.table_data().Columns().Key(i)) &&
                      (table_data().Columns().Label(i) == t.table_data().Columns().Label(i));
            }

            if(ret)
            {
                if(RowCount() == t.RowCount())
                {
                    for(int i = 0; ret && i < RowCount(); i++)
                    {
                        ret = table_data().Rows().At(i).Equals(
                                t.table_data().Rows()[i]);
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

DataObjects::DataRowCollection &
        DataObjects::DataTable::Rows()
{
    return table_data().Rows();
}

const DataObjects::DataRowCollection &DataObjects::DataTable::Rows() const
{
    return table_data().Rows();
}

DataRow &DataObjects::DataTable::add_new_row(const Custom::GVariantList &values)
{
    DataRow dr(*this, values);

    init_new_row(dr);

    return add_row(dr);
}

DataRow &DataTable::add_row(const DataRow &r)
{
    DataRow &ret = Rows().Add(r);

    if(_key_violations())
    {
        Rows().Remove(Rows().Count() - 1);

        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "The primary key would be violated");
    }

    return ret;
}

DataObjects::DataRow &DataObjects::DataTable::import_row(const DataObjects::DataRow &r)
{
    DataRow tmpr(r);
    r.CloneTo(tmpr);
    tmpr.row_data().Table() = *this;
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
    return Rows().Count();
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

    if(ret == -1)
        THROW_NEW_GUTIL_EXCEPTION(Core::IndexOutOfRangeException,
                                  QString("The column key '%1' does not exist in the %2 table")
                                  .arg(key).arg(table_data().GetName()).toStdString());

    return ret;
}

DataObjects::DataSet DataObjects::DataTable::Set() const
{
    return DataSet(table_data().GetDataSet());
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
    return table_data().GetName();
}

void DataObjects::DataTable::SetTableName(const QString &n)
{
    table_data().SetName(n);
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
    Custom::GVariant(ColumnKeys()).WriteXml(sw);
    Custom::GVariant(ColumnLabels()).WriteXml(sw);

    for(int i = 0; i < RowCount(); i++)
        table_data().Rows()[i].WriteXml(sw);

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
        if(sr.name() != DATATABLE_XML_ID){
            Core::XmlException ex("Unrecognized XML Node");
            ex.SetData("elt_name", sr.name().toString().toStdString());
            THROW_GUTIL_EXCEPTION(ex);
        }

        int sz = sr.attributes().at(0).value().toString().toInt();

        table_data().SetName(Utils::QStringHelpers::fromBase64(
                sr.attributes().at(1).value().toString()));

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
    return table_data().KeyColumns();
}

void DataObjects::DataTable::AddKeyColumn(int k)
{
    if(0 <= k && k < ColumnCount())
    {
        table_data().KeyColumns().insert(k);

        if(_key_violations())
        {
            table_data().KeyColumns().remove(k);

            THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
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
        table_data().KeyColumns().remove(k);

        if(_key_violations())
        {
            table_data().KeyColumns().insert(k);

            THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                      "The table's primary key would be violated");
        }
    }
}

void DataObjects::DataTable::RemoveKeyColumn(const QString &k)
{
    RemoveKeyColumn(GetColumnIndex(k));
}

void DataObjects::DataTable::validate_new_row(const DataRow &r)
        throw(Core::ValidationException)
{
    if(*this != r.Table())
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "The row does not belong to this table.  "
                                  "If you still want to add it, then call 'ImportRow' "
                                  "on the parent table.");
    else if(Rows().Contains(r))
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "Row already exists in the table");
}

bool DataObjects::DataTable::IsDirty() const{
    bool ret = table_data().IsDirty();

    // If the table itself was not marked dirty, it could still be dirty
    //  if any of the rows are dirty
    for(int i = 0; !ret && i < RowCount(); i++)
        ret = Rows()[i].IsDirty();

    return ret;
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
    table_data().SharedLock().lockForRead();
}

void DataObjects::DataTable::LockForWrite()
{
    table_data().SharedLock().lockForWrite();
}

bool DataObjects::DataTable::TryLockForRead()
{
    return table_data().SharedLock().tryLockForRead();
}

bool DataObjects::DataTable::TryLockForWrite()
{
    return table_data().SharedLock().tryLockForWrite();
}

void DataObjects::DataTable::Unlock()
{
    table_data().SharedLock().unlock();
}

DataRow &DataObjects::DataTable::FindRow(const QMap<int, GVariant> &keycolumn_value_mapping)
        throw(Core::NotFoundException)
{
    if(keycolumn_value_mapping.count() == 0)
        THROW_NEW_GUTIL_EXCEPTION(Core::NotFoundException,
                                  "You didn't provide any keys by which to search");

    for(int i = 0; i < RowCount(); i++)
    {
        bool found = true;
        foreach(int c, keycolumn_value_mapping.keys())
        {
            if(!found)
                break;

            found = found && keycolumn_value_mapping[c] == At(i)[c];
        }

        if(found)
            return At(i);
    }

    Core::NotFoundException ex("The row was not found.  Check exception data for search criteria.");
    int cnt = 1;
    foreach(int c, keycolumn_value_mapping.keys())
        ex.SetData(QString("k%1").arg(cnt++).toStdString(),
                   QString("Column %1: %2").arg(c).arg(keycolumn_value_mapping[c].toString())
                   .toStdString());

    THROW_GUTIL_EXCEPTION(ex);
}

const DataRow &DataObjects::DataTable::FindRow(const QMap<int, GVariant> &keycolumn_value_mapping) const
        throw(Core::NotFoundException)
{
    if(keycolumn_value_mapping.count() == 0)
        THROW_NEW_GUTIL_EXCEPTION(Core::NotFoundException,
                                  "You didn't provide any keys by which to search");

    for(int i = 0; i < RowCount(); i++)
    {
        bool found = true;
        foreach(int c, keycolumn_value_mapping.keys())
        {
            if(!found)
                break;

            found = found && keycolumn_value_mapping[c] == At(i)[c];
        }

        if(found)
            return At(i);
    }

    Core::NotFoundException ex("The row was not found.  Check exception data for search criteria.");
    int cnt = 1;
    foreach(int c, keycolumn_value_mapping.keys())
        ex.SetData(QString("k%1").arg(cnt++).toStdString(),
                   QString("Column %1: %2").arg(c).arg(keycolumn_value_mapping[c].toString())
                   .toStdString());

    THROW_GUTIL_EXCEPTION(ex);
}

DataRow &DataObjects::DataTable::FindRow(int col, const Custom::GVariant &v)
{
    QMap<int, Custom::GVariant> m;
    m.insert(col, v);
    return FindRow(m);
}

const DataRow &DataObjects::DataTable::FindRow(int col, const Custom::GVariant &v) const
{
    QMap<int, Custom::GVariant> m;
    m.insert(col, v);
    return FindRow(m);
}
