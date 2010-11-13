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
    set_data(new SetData(this))
{
}

DataObjects::DataSet::~DataSet(){}

DataObjects::DataTableCollection &DataObjects::DataSet::Tables()
{
    return set_data->tables;
}

int DataObjects::DataSet::TableCount() const
{
    return set_data->tables.Count();
}

DataObjects::DataTable &DataObjects::DataSet::operator [](int i)
{
    return set_data->tables[i];
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

int DataObjects::DataSet::GetTableIndex(const QString &table_name) const
{
    int ret = -1;
    for(int i = 0; i < TableCount(); i++)
    {
        if(set_data->tables.Value(i).Name() == table_name)
        {
            ret = i;
            break;
        }
    }
    return ret;
}


void DataObjects::DataSet::CommitChanges()
{
    commit_reject_changes(true);
    IUpdatable::CommitChanges();
}

void DataObjects::DataSet::RejectChanges()
{
    commit_reject_changes(false);
    IUpdatable::RejectChanges();
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



#define DATASET_XML_ID "DataSet"

void DataObjects::DataSet::WriteXml(QXmlStreamWriter &sw) const
{
    sw.writeStartElement(DATASET_XML_ID);
    sw.writeAttribute("s", QString("%1").arg(TableCount()));

    for(int i = 0; i < TableCount(); i++)
        set_data->tables.Value(i).WriteXml(sw);

    sw.writeEndElement();
}

void DataObjects::DataSet::ReadXml(QXmlStreamReader &sr)
        throw(GUtil::Core::XmlException)
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


