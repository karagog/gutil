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


// Just like a GVariant, but you can also register your own callback functions
//  that get called before and after the value changes.

// To stop a value from being changed in the 'AboutToChange' method, throw an exception

template <class ObservingClass> class ObservableGVariant :
        public UpdatableGVariant
{
public:

    inline ObservableGVariant(const GVariant &gv = GVariant())
        :UpdatableGVariant(gv){ _init(); }

    ObservableGVariant(const ObservableGVariant &ogv)
        :UpdatableGVariant(ogv)
    {
        _instance = ogv._instance;
        _value_about_to_change_callback = ogv._value_about_to_change_callback;
        _value_changed_callback = ogv._value_changed_callback;
    }

    inline void SetValueAboutToChangeFunction(ObservingClass *instance,
            void (ObservingClass::*func)(const GVariant &current, const GVariant &future)){
        _instance = instance;
        _value_about_to_change_callback = func;
    }

    inline void SetValueChangedFunction(ObservingClass *instance,
            void (ObservingClass::*func)(const GVariant &oldval, const GVariant &newval)){
        _instance = instance;
        _value_changed_callback = func;
    }


private:

    // These are virtual overrides to the UpdataGVariant versions
    void value_about_to_change(const GVariant &o, const GVariant &t){
        UpdatableGVariant::value_about_to_change(o, t);

        if(_value_about_to_change_callback)
            (_instance->*_value_about_to_change_callback)(o, t);
    }

    void value_changed(const GVariant &o, const GVariant &t){
        UpdatableGVariant::value_changed(o, t);

        if(_value_changed_callback)
            (_instance->*_value_changed_callback)(o, t);
    }

    // A pointer to an instance to call the callbacks on
    ObservingClass *_instance;
    void (ObservingClass::*_value_about_to_change_callback)(const GVariant &current, const GVariant &future);
    void (ObservingClass::*_value_changed_callback)(const GVariant &oldvalue, const GVariant &newvalue);

    void _init(){
        _instance = 0;
        _value_about_to_change_callback = 0;
        _value_changed_callback = 0;
    }

};


GUTIL_END_NAMESPACE;

#endif // OBSERVABLEGVARIANT_H
