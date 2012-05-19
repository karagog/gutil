/*Copyright 2010-2012 George Karagoulis
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_PAIR_H
#define GUTIL_PAIR_H

#include "gutil_macros.h"

NAMESPACE_GUTIL1(DataObjects);


/** Represents a pair of items.
    The items are not guarded by accessor functions; this is just basically a data container
    to hold two disparate pieces of data.
*/
template<class T1, class T2 = T1>
class Pair
{
public:

    /** The type of the first item. */
    typedef T1 FirstType;
    /** The type of the second item. */
    typedef T2 SecondType;

    inline Pair(){}
    inline Pair(const T1 &first)
        :First(first){}
    inline Pair(const T1 &first, const T2 &second)
        :First(first), Second(second){}

    /** The first piece of data. */
    T1 First;
    /** The second piece of data. */
    T2 Second;

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_PAIR_H
