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
#include "datatablecollection.h"
#include "datatablecollectionbase.h"
#include <QCoreApplication>
#include <QVariantList>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
using namespace GUtil;

DataObjects::DataSet::DataSet(int num_tables)
    :QAbstractItemModel(qApp),
    _set_data(new SharedSetData(num_tables))
{}

DataObjects::DataSet::DataSet(const DataObjects::DataSet &o)
    :QAbstractItemModel(qApp)
{
    *this = o;
}

DataObjects::DataSet::DataSet(SharedSetData *sd)
    :QAbstractItemModel(qApp),
    _set_data(sd)
{}

DataObjects::DataSet::~DataSet(){}

DataObjects::DataSet &DataObjects::DataSet::operator =(const DataObjects::DataSet &o)
{
    _set_data = o._set_data;
}

DataObjects::DataTableCollection &DataObjects::DataSet::Tables() const
{
    return set_data().Tables();
}

int DataObjects::DataSet::TableCount() const
{
    return Tables().Count();
}

DataObjects::DataTable &DataObjects::DataSet::AddTable(const DataTable &t)
{
    return Tables().Add(t);
}

void DataObjects::DataSet::Clear()
{
    set_data().Tables().Clear();
}

DataObjects::DataTable &DataObjects::DataSet::operator [](int i)
{
    return set_data().Tables()[i];
}

const DataObjects::DataTable &DataObjects::DataSet::operator [](int i) const
{
    return set_data().Tables()[i];
}

DataObjects::DataTable &DataObjects::DataSet::operator [](const QString &name)
{
    return Tables()[GetTableIndex(name)];
}

const DataObjects::DataTable &DataObjects::DataSet::operator [](const QString &name) const
{
    return Tables()[GetTableIndex(name)];
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
                 set_data().Tables().At(i).Equals(d.set_data().Tables().At(i))))
                break;
        }
    }
    return ret;
}

bool DataObjects::DataSet::Contains(const DataTable &t) const
{
    return Tables().Contains(t);
}

int DataObjects::DataSet::GetTableIndex(const QString &table_name) const
{
    int ret = -1;
    for(int i = 0; i < TableCount(); i++)
    {
        if(set_data().Tables().At(i).Name() == table_name)
        {
            ret = i;
            break;
        }
    }
    return ret;
}

DataObjects::SharedSetData &DataObjects::DataSet::set_data() const
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
        set_data().Tables().At(i).WriteXml(sw);

    write_xml_protected(sw);

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
            DataTable t(*this);
            t.ReadXml(sr);
            Tables().Add(t);
        }

        read_xml_protected(sr);

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




