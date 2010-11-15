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
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
using namespace GUtil;

DataObjects::DataSet::DataSet(QObject *parent)
    :QAbstractItemModel(parent == 0 ? qApp : parent),
    _set_data(new SetData(this))
{}

DataObjects::DataSet::DataSet(const DataObjects::DataSet &o)
{
    *this = o;
}

DataObjects::DataSet::~DataSet(){}

DataObjects::DataSet &DataObjects::DataSet::operator =(const DataObjects::DataSet &o)
{
    _set_data = o._set_data;
}

DataObjects::DataTableCollection &DataObjects::DataSet::Tables()
{
    return _set_data->tables;
}

int DataObjects::DataSet::TableCount() const
{
    return _set_data->tables.Count();
}

DataObjects::DataTable &DataObjects::DataSet::AddTable(const DataTable &t)
{
    return Tables().Add(t);
}

void DataObjects::DataSet::Clear()
{
    Tables().Resize(0);
}

DataObjects::DataTable &DataObjects::DataSet::operator [](int i)
{
    return _set_data->tables[i];
}

DataObjects::DataTable &DataObjects::DataSet::operator [](const QString &name)
{
    int ind = GetTableIndex(name);
    if(ind == -1)
        THROW_NEW_GUTIL_EXCEPTION(Core::IndexOutOfRangeException,
                                  QString("Table '%1' does not exist in dataset")
                                  .arg(name).toStdString());

    return Tables()[ind];
}

bool DataObjects::DataSet::operator ==(const DataObjects::DataSet &d) const
{
    return _set_data == d._set_data;
}

bool DataObjects::DataSet::operator !=(const DataObjects::DataSet &d) const
{
    return !(*this == d);
}

bool DataObjects::DataSet::Equals(const DataObjects::DataSet &d) const
{
    bool ret = false;
    if(_set_data == d._set_data)
        ret = true;
    else
    {
        for(int i = 0; i < TableCount(); i++)
        {
            if(!(ret =
                 _set_data->tables.Value(i).Equals(d._set_data->tables.Value(i))))
                break;
        }
    }
    return ret;
}

int DataObjects::DataSet::GetTableIndex(const QString &table_name) const
{
    int ret = -1;
    for(int i = 0; i < TableCount(); i++)
    {
        if(_set_data->tables.Value(i).Name() == table_name)
        {
            ret = i;
            break;
        }
    }
    return ret;
}

DataObjects::DataSet::SetData &DataObjects::DataSet::set_data() const
{
    return *_set_data;
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

DataObjects::DataSet DataObjects::DataSet::Clone() const
{
    DataSet ret(*this);
    return CloneTo(ret);
}

DataObjects::DataSet &DataObjects::DataSet::CloneTo(DataObjects::DataSet &o) const
{
    o = *this;
    o._set_data.detach();
    return o;
}



#define DATASET_XML_ID "DataSet"

void DataObjects::DataSet::WriteXml(QXmlStreamWriter &sw) const
{
    sw.writeStartElement(DATASET_XML_ID);
    sw.writeAttribute("s", QString("%1").arg(TableCount()));

    for(int i = 0; i < TableCount(); i++)
        _set_data->tables.Value(i).WriteXml(sw);

    sw.writeEndElement();
}

void DataObjects::DataSet::ReadXml(QXmlStreamReader &sr)
        throw(GUtil::Core::XmlException)
{
    FailIfReadOnly();

    Clear();

    if(sr.readNextStartElement())
    {
        if(sr.name() != DATASET_XML_ID)
            THROW_NEW_GUTIL_EXCEPTION( Core::XmlException, "Unrecognized XML Node" );

        int len = sr.attributes().at(0).value().toString().toInt();
        for(int i = 0; i < len; i++)
        {
            DataTable t(this);
            t.ReadXml(sr);
            Tables().Add(t);
        }

        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != DATASET_XML_ID);
    }
}



QModelIndex DataObjects::DataSet::index(int, int, const QModelIndex &) const
{

}

QModelIndex DataObjects::DataSet::parent(const QModelIndex &child) const
{

}

int DataObjects::DataSet::rowCount(const QModelIndex &) const
{

}

int DataObjects::DataSet::columnCount(const QModelIndex &) const
{

}

QVariant DataObjects::DataSet::data(const QModelIndex &index, int role) const
{
    QVariant ret;


    return ret;
}

bool DataObjects::DataSet::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(IsReadOnly())
        return false;
}

bool DataObjects::DataSet::insertRows(int row, int count, const QModelIndex &parent)
{

}

bool DataObjects::DataSet::removeRows(int row, int count, const QModelIndex &parent)
{

}

QVariant DataObjects::DataSet::headerData(
        int section,
        Qt::Orientation orientation,
        int role) const
{

}

Qt::ItemFlags DataObjects::DataSet::flags(const QModelIndex &index) const
{

}





DataObjects::DataSet::SetData::SetData(DataSet *ds)
    :tables(ds)
{

}

DataObjects::DataSet::SetData::SetData(const DataObjects::DataSet::SetData &o)
    :tables(o.tables)
{
}


