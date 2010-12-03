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

#ifndef OBSERVABLEGVARIANT_H
#define OBSERVABLEGVARIANT_H

#include "updatablegvariant.h"

GUTIL_BEGIN_NAMESPACE( Custom );


// A base class that is required to observe an ObservableGVariant
class GVariantObserver;


// Just like a GVariant, but you can also register your own callback functions
//  that get called before and after the value changes.

// To stop a value from being changed in the 'AboutToChange' method, throw an exception

class ObservableGVariant :
        public UpdatableGVariant
{
public:

    ObservableGVariant(const GVariant &gv = GVariant());
    ObservableGVariant(const ObservableGVariant &);

    void SetObserver(GVariantObserver *o);


private:

    // These are virtual overrides to the UpdataGVariant versions
    void value_about_to_change(const GVariant &o, const GVariant &t);
    void value_changed(const GVariant &o, const GVariant &t);

    // A pointer to an instance to call the callbacks on
    GVariantObserver *_observer;

};



// Any class that wants to observe a GVariant must implement this base class
class GVariantObserver
{
    friend class ObservableGVariant;

protected:

    virtual void gvariant_value_about_to_change(const GVariant &, const GVariant &){}
    virtual void gvariant_value_changed(const GVariant &, const GVariant &){}

};


GUTIL_END_NAMESPACE;

#endif // OBSERVABLEGVARIANT_H
