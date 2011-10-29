/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_ATOMIC_H
#define GUTIL_ATOMIC_H

#include "gutil_macros.h"

GUTIL_BEGIN_CORE_NAMESPACE(Utils);


/** A static class to declare helpful atomic operations, implemented in assembly. */
class Atomic
{
public:

    /** Increments the integer pointed to.
        \return True if the integer after the operation is not equal to 0
    */
    static bool Increment(GINT32 *i);

    /** Decrements the integer pointed to.
        \return True if the integer after the operation is not equal to 0
    */
    static bool Decrement(GINT32 *i);

    /** Adds the value to the integer pointed to.
        \return The integer's value before adding
    */
    static GINT32 FetchAndAdd(GINT32 *i, GINT32 n);

    /** Adds the value to the integer pointed to.
        \return The integer's value after adding
    */
    static GINT32 AddAndFetch(GINT32 *i, GINT32 n);

};


GUTIL_END_CORE_NAMESPACE

#endif // GUTIL_ATOMIC_H
