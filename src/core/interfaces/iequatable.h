/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef IEQUATABLE_H
#define IEQUATABLE_H

#include "gutil_macros.h"

NAMESPACE_GUTIL1( Interfaces );


template <class T> class IEquatable
{
public:

    virtual bool Equals(const T &) const = 0;

    bool NotEquals(const T &o) const{
        return !Equals(o);
    }

    virtual ~IEquatable(){}
};


END_NAMESPACE_GUTIL1

#endif // IEQUATABLE_H
