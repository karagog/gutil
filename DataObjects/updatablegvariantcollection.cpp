/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "updatablegvariantcollection.h"
#include "gvariantcollection.h"
GUTIL_USING_NAMESPACE( DataObjects );
GUTIL_USING_NAMESPACE( Custom );


UpdatableGVariantCollection::UpdatableGVariantCollection(int size, const GVariantList &v)
    :ResizableCollection<Custom::UpdatableGVariant>(size),
    _observer(0)
{
    for(int i = 0; i < v.count() && i < size; i++)
        At(i) = v[i];

    _init();
}

UpdatableGVariantCollection::UpdatableGVariantCollection(const UpdatableGVariantCollection &v)
    :ResizableCollection<Custom::UpdatableGVariant>(v),
    Custom::UpdatableGVariant::Observer(),
    Core::Interfaces::IUpdatable(),
    _observer(0)
{
    _init();
}

void UpdatableGVariantCollection::_init()
{
    for(int i = 0; i < Count(); i++)
        At(i).SetValueObserver(this);
}

void UpdatableGVariantCollection::on_add(Custom::UpdatableGVariant *ogv)
{
    ogv->SetValueObserver(this);
}

void UpdatableGVariantCollection::commit_reject_changes(bool commit)
{
    for(int i = 0; i < Count(); i++)
    {
        if(commit)
            At(i).CommitChanges();
        else
            At(i).RejectChanges();
    }
}

void UpdatableGVariantCollection::value_about_to_change(
        const GVariant &orig, const GVariant &newval)
{
    // Have to reset _index_mem to -1 so that it will update it in the 'find' function
    _index_mem = -1;

    if(_observer)
    {
        _find_changed_index(orig);
        _observer->value_about_to_change(_index_mem, newval);
    }
}

void UpdatableGVariantCollection::value_changed(
        const GVariant &oldval, const GVariant &newval)
{
    if(_observer)
    {
        _find_changed_index(newval);

        // We reuse _index_mem so we don't have to find the same index twice
        _observer->value_changed(_index_mem, oldval);
    }
}

void UpdatableGVariantCollection::_find_changed_index(const Custom::GVariant &v)
{
    // Find the index of the value which is about to change
    for(int i = 0; _index_mem == -1 && i < Count(); i++)
        if(&At(i) == &v)
            _index_mem = i;

    Q_ASSERT(_index_mem != -1);
}
