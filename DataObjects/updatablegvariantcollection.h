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
#include "gvariantcollection.h"
#include "Custom/updatablegvariant.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/ivalueobserver.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class UpdatableGVariantCollection :
        public ResizableCollection< Custom::UpdatableGVariant >,
        public Core::Interfaces::IValueObserver<Custom::GVariant>,
        public Core::Interfaces::IUpdatable
{
public:
    UpdatableGVariantCollection(int size = 0);
    UpdatableGVariantCollection(const UpdatableGVariantCollection &);

protected:

    // Classes may derive from this one to implement their own functions
    virtual void on_value_about_to_change(int index, const Custom::GVariant &newvalue);
    virtual void on_value_changed(int index, const Custom::GVariant &previousvalue);

    // Every new observable gvariant we add must be initialized
    virtual void on_add(Custom::UpdatableGVariant *);

    // IUpdatable interface:
    virtual void commit_reject_changes(bool commit);


private:

    // GVariantObserver interface:
    void value_about_to_change(const Custom::GVariant &, const Custom::GVariant &);
    void value_changed(const Custom::GVariant &, const Custom::GVariant &);

    int _index_mem;

    void _init();

};


GUTIL_END_NAMESPACE;

#endif // UPDATABLEGVARIANTCOLLECTION_H
