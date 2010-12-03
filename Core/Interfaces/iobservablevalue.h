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

#ifndef IOBSERVABLEVALUE_H
#define IOBSERVABLEVALUE_H

#include "ivalueobserver.h"

GUTIL_BEGIN_CORE_NAMESPACE( Interfaces );


template <class T> class IObservableValue
{
public:

    IObservableValue(IValueObserver<T> *vo = 0)
        :_value_observer(vo)
    {}

    void SetValueObserver(IValueObserver<T> *vo){
        _value_observer = vo;
    }


protected:

    // Derived classes call these functions to notify their observer (if any) that
    //  they've changed
    void notify_value_about_to_change(const T &before, const T &after){
        if(_value_observer)
            _value_observer->value_about_to_change(before, after);
    }

    void notify_value_changed(const T &before, const T &after){
        if(_value_observer)
            _value_observer->value_changed(before, after);
    }


private:

    IValueObserver<T> *_value_observer;

};


GUTIL_END_CORE_NAMESPACE;

#endif // IOBSERVABLEVALUE_H
