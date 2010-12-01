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

#ifndef IINDEXABLE_H
#define IINDEXABLE_H

#include "gutil_macros.h"

GUTIL_BEGIN_CORE_NAMESPACE( Interfaces );


template <class T> class IIndexable
{
public:

    virtual T& At(int) = 0;
    virtual const T & At(int) const = 0;

    T& operator [](int i){
        return At(i);
    }

    virtual const T& operator [](int i) const{
        return At(i);
    }
};


GUTIL_END_CORE_NAMESPACE

#endif // IINDEXABLE_H
