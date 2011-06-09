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

#include "tabledata.h"
GUTIL_USING_NAMESPACE( DataObjects );

TableData::TableData()
    :Columns(this),
      Rows(this)
{}

TableData::TableData(const TableData &d)
    :IUpdatable(),
      Columns(d.Columns),
      Rows(this, d.Rows),
      Name(d.Name),
      KeyColumns(d.KeyColumns)
{}

void TableData::commit_reject_changes(bool commit)
{
    for(int i = 0; i < Rows.Count(); i++)
        Rows[i].row_data().CommitChanges(commit);
}

int TableData::GetColumnIndex(const QString &key) const
{
    int ret = -1;
    for(int i = 0; i < Columns.Count(); i++)
        if(Columns.Keys()[i] == key)
        {
            ret = i;
            break;
        }

    if(ret == -1)
        THROW_NEW_GUTIL_EXCEPTION2(Core::IndexOutOfRangeException,
                                  QString("The column key '%1' does not exist in the %2 table")
                                  .arg(key).arg(Name).toStdString());

    return ret;
}
