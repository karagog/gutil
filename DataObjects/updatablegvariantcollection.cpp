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


UpdatableGVariantCollection::UpdatableGVariantCollection(int size)
    :ResizableCollection< Custom::ObservableGVariant<UpdatableGVariantCollection> >(size)
{}

UpdatableGVariantCollection::UpdatableGVariantCollection(const ResizableCollection< Custom::ObservableGVariant<UpdatableGVariantCollection> > &v)
    : ResizableCollection< Custom::ObservableGVariant<UpdatableGVariantCollection> >(v)
{}

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
