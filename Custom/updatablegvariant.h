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

#ifndef UPDATABLEGVARIANT_H
#define UPDATABLEGVARIANT_H

#include "gvariant.h"
#include "Core/Interfaces/iupdatable.h"
#include "Core/Interfaces/iobservablevalue.h"

GUTIL_BEGIN_NAMESPACE( Custom );


// Just like a GVariant, except it recognizes when it's been changed and you can
//  check by calling 'IsDirty()' on it.

class UpdatableGVariant :
        public GVariant,
        public Core::Interfaces::IUpdatable,
        public Core::Interfaces::IObservableValue<GVariant>
{
public:

    UpdatableGVariant(const GVariant &gv = GVariant());
    UpdatableGVariant(const UpdatableGVariant &ogv);


    // We override these functions from QVariant, so we can call our callback functions
    UpdatableGVariant &operator = (const GVariant &o);
    void clear();
    void convert(Type t);
    template <class U> void setValue(const U &value);


protected:

    // Override this to know if our value has changed
    virtual void notify_value_changed(const GVariant &, const GVariant &);

    // IUpdatable interface
    virtual void commit_reject_changes(bool commit);


private:

    GVariant _backup;
};


GUTIL_END_NAMESPACE;

#endif // UPDATABLEGVARIANT_H
