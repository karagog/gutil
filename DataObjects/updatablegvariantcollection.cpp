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
GUTIL_USING_NAMESPACE( DataObjects );


UpdatableGVariantCollection::UpdatableGVariantCollection(int size)
    :ResizableCollection< Custom::ObservableGVariant<UpdatableGVariantCollection> >(size),
    _backup_collection(new GVariantCollection(size))
{
    // on_add is a virtual function, so it doesn't get called in ResizableCollection's constructor
    for(int i = 0; i < Count(); i++)
        on_add(&At(i));
}

UpdatableGVariantCollection::UpdatableGVariantCollection(const ResizableCollection< Custom::ObservableGVariant<UpdatableGVariantCollection> > &v)
    : ResizableCollection< Custom::ObservableGVariant<UpdatableGVariantCollection> >(v){}

void UpdatableGVariantCollection::Resize(int size)
{
    ResizableCollection< Custom::ObservableGVariant<UpdatableGVariantCollection> >::Resize(size);

    _backup_collection->Resize(size);
}

void UpdatableGVariantCollection::value_about_to_change_func(const Custom::GVariant &, const Custom::GVariant &){}

void UpdatableGVariantCollection::value_changed_func(const Custom::GVariant &o, const Custom::GVariant &t)
{
    if(o != t)
        SetDirty(true);
}

void UpdatableGVariantCollection::on_add(Custom::ObservableGVariant<UpdatableGVariantCollection> *ogv)
{
    ogv->SetValueAboutToChangeFunction(this, &UpdatableGVariantCollection::value_about_to_change_func);
    ogv->SetValueChangedFunction(this, &UpdatableGVariantCollection::value_changed_func);
}

void UpdatableGVariantCollection::commit_reject_changes(bool commit)
{
    if(commit)
        for(int i = 0; i < Count(); i++)
            _backup_collection->At(i) = At(i);
    else
        for(int i = 0; i < Count(); i++)
            At(i) = _backup_collection->At(i);
}
