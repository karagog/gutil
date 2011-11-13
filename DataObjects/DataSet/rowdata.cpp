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

#include "rowdata.h"
#include "datatable.h"
#include "tabledata.h"
#include "DataObjects/updatablegvariantcollection.h"
using namespace GUtil;

DataObjects::RowData::RowData(TableData *t,
                              const Custom::GVariantVector &vals)
    :Tuple(t->Columns.Count(), vals),
      Table(t)
{
    Tuple.SetCollectionObserver(this);
}

DataObjects::RowData::RowData(const DataObjects::RowData &o)
    :Core::Interfaces::IUpdatable(o),
      Tuple(o.Tuple),
      Table(o.Table)
{
    Tuple.SetCollectionObserver(this);
}

void DataObjects::RowData::value_about_to_change(int index, const Custom::GVariant &newvalue)
{
    if(Tuple[index] != newvalue)
        MakeDirty();

    // Check for primary key violations
    if(Table->KeyColumns.count() == 0 ||
       !Table->KeyColumns.contains(index))
        return;

    bool key_violation = false;
    for(int i = 0; !key_violation && i < Table->Rows.Count(); i++)
    {
        const DataRow &cur_row = Table->Rows[i];

        if(&cur_row.row_data() == this)
            continue;

        key_violation = true;
        foreach(int k, Table->KeyColumns)
        {
            if(k == index)
                key_violation = key_violation && (newvalue == cur_row[k]);
            else
                key_violation = key_violation && (Tuple.At(k) == cur_row[k]);

            if(!key_violation)
                break;
        }
    }

    if(key_violation)
        THROW_NEW_GUTIL_EXCEPTION2(Core::ValidationException,
                                  QString("Primary key violation trying to set index %1 "
                                          "to the value \"%2\"")
                                  .arg(index)
                                  .arg(newvalue.toString())
                                  .toAscii().constData());
}

void DataObjects::RowData::on_make_dirty()
{
    Table->MakeDirty();
}
