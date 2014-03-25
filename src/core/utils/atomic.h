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

#ifndef GUTIL_ATOMIC_H
#define GUTIL_ATOMIC_H

#include "gutil_macros.h"

NAMESPACE_GUTIL;


/** A class to declare helpful atomic operations, implemented in assembly. */
class AtomicInt
{
    GINT32 m_value;
public:

    /** Constructs the atomic int with initial value 0. */
    AtomicInt() :m_value(0){}

    /** Constructs the atomic int with the given initial value. */
    AtomicInt(GINT32 i) :m_value(i){}
    
    AtomicInt(const AtomicInt &i) :m_value(i.Value()){}

    /** The integer's current value. */
    GINT32 Value() const{ return m_value; }

    /** Increments the integer pointed to.
        \return True if the integer after the operation is not equal to 0
    */
    bool Increment();

    /** Decrements the integer pointed to.
        \return True if the integer after the operation is not equal to 0
    */
    bool Decrement();

    /** Adds the value to the integer pointed to.
        \return The integer's value before adding
    */
    GINT32 FetchAndAdd(GINT32 n);

    /** Adds the value to the integer pointed to.
        \return The integer's value after adding
    */
    GINT32 AddAndFetch(GINT32 n);


    inline bool operator == (GINT32 v) const{ return m_value == v; }
    inline bool operator != (GINT32 v) const{ return m_value != v; }

    AtomicInt &operator = (GINT32 i){ m_value = i; return *this; }

    inline operator GINT32() const{ return m_value; }

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_ATOMIC_H
