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

#ifndef UPDATABLEGVARIANTCOLLECTION_H
#define UPDATABLEGVARIANTCOLLECTION_H

#include "collection.h"
#include "gvariantcollection.h"
#include "Custom/updatablegvariant.h"
#include "Core/Interfaces/iupdatable.h"

GUTIL_BEGIN_NAMESPACE( DataObjects );


class GVariantCollectionObserver;

class UpdatableGVariantCollection :
        public ResizableCollection< Custom::UpdatableGVariant >,
        public Custom::UpdatableGVariant::Observer,
        public Core::Interfaces::IUpdatable
{
public:

    UpdatableGVariantCollection(GUINT32 size = 0, const Custom::GVariantVector &vals = Custom::GVariantVector());
    UpdatableGVariantCollection(const UpdatableGVariantCollection &);


    // Classes who wish to observe the gvariantcollection
    //   may derive from this and set themselves as the gvariant observer
    class Observer
    {
        friend class UpdatableGVariantCollection;
    protected:

        virtual void value_about_to_change(int, const Custom::GVariant &){}
        virtual void value_changed(int, const Custom::GVariant &){}

    };

    inline void SetCollectionObserver(Observer *o){ _observer = o; }


protected:

    // Every new observable gvariant we add must be initialized
    virtual void on_add(Custom::UpdatableGVariant *);

    // IUpdatable interface:
    virtual void commit_reject_changes(bool commit);


private:

    // GVariantObserver interface:
    void value_about_to_change(const Custom::GVariant &, const Custom::GVariant &);
    void value_changed(const Custom::GVariant &, const Custom::GVariant &);

    void _init();

    // This function updates _index_mem to point to the index being changed
    void _find_changed_index(const Custom::GVariant &);
    int _index_mem;

    Observer *_observer;

};






GUTIL_END_NAMESPACE;

#endif // UPDATABLEGVARIANTCOLLECTION_H
