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

#include "gvariant.h"

GUTIL_BEGIN_NAMESPACE( Custom );


// Just like a GVariant, but you can also register your own callback functions
//  that get called before and after the value changes.

// To stop a value from being changed in the 'AboutToChange' method, throw an exception

template <class T> class ObservableGVariant :
        public GVariant
{
public:

    inline ObservableGVariant(const GVariant &gv = GVariant())
        :GVariant(gv){ _init(); }

    ObservableGVariant(const ObservableGVariant &ogv)
        :GVariant(ogv)
    {
        _instance = ogv._instance;
        _value_about_to_change_callback = ogv._value_about_to_change_callback;
        _value_changed_callback = ogv._value_changed_callback;
    }

    inline void SetValueAboutToChangeFunction(T *instance,
            void (T::*func)(const GVariant &current, const GVariant &future)){
        _instance = instance;
        _value_about_to_change_callback = func;
    }

    inline void SetValueChangedFunction(T *instance,
            void (T::*func)(const GVariant &oldval, const GVariant &newval)){
        _instance = instance;
        _value_changed_callback = func;
    }


    // We override these functions from QVariant, so we can call our callback functions
    ObservableGVariant &operator = (const GVariant &o){
        GVariant tmp(*this);

        value_about_to_change(tmp, o);
        ((QVariant &)*this) = o;
        value_changed(tmp, o);
        return *this;
    }

    void clear(){
        GVariant tmp(*this);
        GVariant c;

        value_about_to_change(tmp, c);
        QVariant::clear();
        value_changed(tmp, c);
    }

    void convert(Type t){
        GVariant tmp1(*this);
        QVariant tmp2(tmp1);
        tmp2.convert(t);

        value_about_to_change(tmp1, tmp2);
        QVariant::convert(t);
        value_changed(tmp1, tmp2);
    }

    template <class U> void setValue(const U &value){
        GVariant tmp(*this);
        GVariant tmp2(value);

        value_about_to_change(tmp, tmp2);
        QVariant::setValue(value);
        value_changed(tmp, tmp2);
    }


private:

    void value_about_to_change(const GVariant &o, const GVariant &t){
        if(_value_about_to_change_callback)
            (_instance->*_value_about_to_change_callback)(o, t);
    }

    void value_changed(const GVariant &o, const GVariant &t){
        if(_value_changed_callback)
            (_instance->*_value_changed_callback)(o, t);
    }

    // A pointer to an instance to call the callbacks on
    T *_instance;
    void (T::*_value_about_to_change_callback)(const GVariant &current, const GVariant &future);
    void (T::*_value_changed_callback)(const GVariant &oldvalue, const GVariant &newvalue);

    void _init(){
        _instance = 0;
        _value_about_to_change_callback = 0;
        _value_changed_callback = 0;
    }

};


GUTIL_END_NAMESPACE;


#endif // OBSERVABLEGVARIANT_H
