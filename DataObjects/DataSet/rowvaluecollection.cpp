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

#include "rowvaluecollection.h"
#include "datarow.h"
#include "sharedrowdata.h"

RowValueCollection::RowValueCollection(SharedRowData *rd, int size)
    :UpdatableGVariantCollection(size),
    _row_data(rd)
{
    _value_about_to_change = 0;
    _value_changed = 0;
}

RowValueCollection::RowValueCollection(const RowValueCollection &o)
    :UpdatableGVariantCollection(o),
    _row_data(o._row_data)
{
    _value_about_to_change = o._value_about_to_change;
    _value_changed = o._value_changed;
}

void RowValueCollection::value_about_to_change(int index, const Custom::GVariant &newval)
{
    if(_value_about_to_change)
        _value_about_to_change(DataRow(_row_data), index, newval);
}

void RowValueCollection::value_changed(int index, const Custom::GVariant &prevval)
{
    if(_value_changed)
        _value_changed(DataRow(_row_data), index, prevval);
}

void RowValueCollection::SetValueAboutToChangeFunction(
        void (*func)(const DataRow &, int, const Custom::GVariant &))
{
    _value_about_to_change = func;
}

void RowValueCollection::SetValueChangedFunction(
        void (*func)(const DataRow &, int, const Custom::GVariant &))
{
    _value_changed = func;
}
