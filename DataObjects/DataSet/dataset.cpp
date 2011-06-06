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
#include "datatable.h"
#include "sharedsetdata.h"
#include "datatablecollection.h"
#include <QVariantList>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
using namespace GUtil;

DataObjects::DataSet::DataSet(int num_tables)
    :ExplicitlySharedObject<SharedSetData>(new SharedSetData(num_tables))
{}

DataObjects::DataSet::DataSet(const DataObjects::DataSet &o)
    :ExplicitlySharedObject<SharedSetData>(o),
      Core::Interfaces::IReadOnlyObject(),
      Core::Interfaces::IUpdatable()
{}

DataObjects::DataSet::DataSet(SharedSetData *sd)
    :ExplicitlySharedObject<SharedSetData>(sd)
{}

DataObjects::DataSet::~DataSet(){}

DataObjects::DataSet &DataObjects::DataSet::operator =(const DataObjects::DataSet &o)
{
    SetExplicitlySharedData(o.GetExplicitlySharedData());
    return *this;
}

const DataObjects::DataTableCollection &DataObjects::DataSet::Tables() const
{
    return set_data().Tables();
}

DataObjects::DataTableCollection &DataObjects::DataSet::Tables()
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

bool DataObjects::DataSet::Equals(const DataObjects::DataSet &d) const
{
    bool ret = false;
    if(*this == d)
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

const DataObjects::SharedSetData &DataObjects::DataSet::set_data() const
{
    return *ExplicitlySharedObject<SharedSetData>::GetExplicitlySharedData();
}

DataObjects::SharedSetData &DataObjects::DataSet::set_data()
{
    return *ExplicitlySharedObject<SharedSetData>::GetExplicitlySharedData();
}

void DataObjects::DataSet::validate_new_table(const DataTable &dt)
{
    if(Tables().Contains(dt))
        THROW_NEW_GUTIL_EXCEPTION2(Core::ValidationException,
                                  "Table already exists in data set");
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
    o.Detach();
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
            THROW_NEW_GUTIL_EXCEPTION2( Core::XmlException, "Unrecognized XML Node" );

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
