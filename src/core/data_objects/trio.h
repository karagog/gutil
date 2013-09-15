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

#ifndef GUTIL_TRIO_H
#define GUTIL_TRIO_H

#include "gutil_macros.h"

NAMESPACE_GUTIL1(DataObjects);


/** Represents a trio of data. */
template<class T1, class T2, class T3>
class Trio
{
public:

    /** The first typed object. */
    T1 First;

    /** The second typed object. */
    T2 Second;

    /** The third typed object. */
    T3 Third;

    /** Declare a default constructor if you don't care about initialization. */
    Trio(){}

    /** Initialize all three members of the trio. */
    Trio(const T1 &f, const T2 &s, const T3 &t)
        :First(f), Second(s), Third(t) {}

};




/** If all three types are the same, use this. */
template<class T> class Trio : public Trio<T, T, T>
{
public:

    /** Declare a default constructor if you don't care about initialization. */
    Trio(){}

    /** Initialize all three members of the trio. */
    Trio(const T &f, const T &s, const T &t)
        :Trio<T, T, T>(f, s, t) {}

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_TRIO_H
