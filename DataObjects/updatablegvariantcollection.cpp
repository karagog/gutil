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

#include "updatablegvariantcollection.h"
#include "gvariantcollection.h"
#include "Custom/observablegvariant.h"
GUTIL_USING_NAMESPACE( DataObjects );
GUTIL_USING_NAMESPACE( Custom );


UpdatableGVariantCollection::UpdatableGVariantCollection(int size)
    :ResizableCollection<Custom::ObservableGVariant>(size)
{
    for(int i = 0; i < Count(); i++)
        At(i).SetObserver(this);
}

UpdatableGVariantCollection::UpdatableGVariantCollection(const ResizableCollection<Custom::ObservableGVariant> &v)
    : ResizableCollection<Custom::ObservableGVariant>(v)
{}

void UpdatableGVariantCollection::on_add(Custom::ObservableGVariant *ogv)
{
    ogv->SetObserver(this);
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

void UpdatableGVariantCollection::_gvariant_value_about_to_change(
        const GVariant &orig, const GVariant &newval)
{
    // Find the index of the value which is about to change
    _index_mem = -1;
    for(int i = 0; _index_mem == -1 && i < Count(); i++)
        if(&At(i) == &orig)
            _index_mem = i;

    Q_ASSERT(_index_mem != -1);

    value_about_to_change(_index_mem, newval);
}

void UpdatableGVariantCollection::_gvariant_value_changed(
        const GVariant &oldval, const GVariant &newval)
{
    // We reuse _index_mem so we don't have to find the same index twice
    value_changed(_index_mem, oldval);
}

void UpdatableGVariantCollection::value_about_to_change(int, const GVariant &){}
void UpdatableGVariantCollection::value_changed(int, const GVariant &){}
