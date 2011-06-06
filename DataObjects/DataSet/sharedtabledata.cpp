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

#include "sharedtabledata.h"
#include "dataset.h"
GUTIL_USING_NAMESPACE( DataObjects );


SharedTableData::SharedTableData()
    :_dataset(new DataSet),
      _columns(this),
      _rows(this)
{}

SharedTableData::SharedTableData(const DataSet &ds)
    :_dataset(new DataSet(ds)),
      _columns(this),
      _rows(this)
{}

SharedTableData::SharedTableData(const SharedTableData &d)
    :GSharedData(d),
      IUpdatable(),
      _dataset(new DataSet(*d._dataset)),
      _columns(d.Columns()),
      _name(d._name),
      _key_columns(d._key_columns),
      _rows(this, d.Rows())
{}

SharedTableData::~SharedTableData()
{
    delete _dataset;
}

void SharedTableData::SetDataSet(const DataSet &ds)
{
    if(_dataset)
        delete _dataset;
    _dataset = new DataSet(ds);
}

void SharedTableData::commit_reject_changes(bool commit)
{
    for(int i = 0; i < Rows().Count(); i++)
        Rows()[i].row_data().CommitChanges(commit);
}
