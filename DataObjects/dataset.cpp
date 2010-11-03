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
#include "datatable.h"
#include <QCoreApplication>
#include <QVariantList>
using namespace GUtil;

DataObjects::DataSet::DataSet()
    :QAbstractItemModel(qApp)
{
    _auto_commit = false;
}

DataObjects::DataSet::~DataSet(){}

void DataObjects::DataSet::CommitChanges()
{
    commit_reject_changes(true);
}

void DataObjects::DataSet::RejectChanges()
{
    commit_reject_changes(false);
}

void DataObjects::DataSet::commit_reject_changes(bool commit)
{
    if(commit)
    {

    }
    else
    {

    }
}

void DataObjects::DataSet::SetAutoCommitChanges(bool val)
{
    _auto_commit = val;
}

bool DataObjects::DataSet::AutoCommitChanges() const
{
    return _auto_commit;
}



void DataObjects::DataSet::WriteXml(QXmlStreamWriter &sw) const
{

}

void DataObjects::DataSet::ReadXml(QXmlStreamReader &sr)
        throw(GUtil::Core::XmlException)
{

}
