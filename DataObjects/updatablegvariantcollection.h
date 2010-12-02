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

#ifndef UPDATABLEGVARIANTCOLLECTION_H
#define UPDATABLEGVARIANTCOLLECTION_H

#include "collection.h"
#include "Custom/observablegvariant.h"
#include "gvariantcollection.h"
#include "Core/Interfaces/iupdatable.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class UpdatableGVariantCollection :
        public ResizableCollection< Custom::ObservableGVariant<UpdatableGVariantCollection> >,
        public Core::Interfaces::IUpdatable
{
public:
    UpdatableGVariantCollection(int size = 0);
    UpdatableGVariantCollection(
            const ResizableCollection< Custom::ObservableGVariant<UpdatableGVariantCollection> > &);

    virtual void Resize(int size);

protected:

    virtual void value_about_to_change_func(const Custom::GVariant &, const Custom::GVariant &);
    virtual void value_changed_func(const Custom::GVariant &, const Custom::GVariant &);


    virtual void on_add(Custom::ObservableGVariant<UpdatableGVariantCollection> *);
    virtual void commit_reject_changes(bool commit);


private:

    GVariantCollection *_backup_collection;

};


GUTIL_END_NAMESPACE;

#endif // UPDATABLEGVARIANTCOLLECTION_H
