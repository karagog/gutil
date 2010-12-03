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

#ifndef IVALUEOBSERVER_H
#define IVALUEOBSERVER_H

#include "gutil_macros.h"

GUTIL_BEGIN_CORE_NAMESPACE( Interfaces );


template <class T> class IValueObserver
{
    template <class U> friend class IObservableValue;

protected:

    virtual void value_about_to_change(const T &before, const T &after){}
    virtual void value_changed(const T &before, const T &after){}

};


GUTIL_END_CORE_NAMESPACE;

#endif // IVALUEOBSERVER_H
