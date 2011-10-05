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

#ifndef GUTIL_ICOMPARER_H
#define GUTIL_ICOMPARER_H

#include "gutil.h"

GUTIL_BEGIN_CORE_NAMESPACE( Interfaces );


/** An abstract interface class to compare two objects of different types */
template <class T, class U> class IComparerDifferent
{
public:

    /** Return -1 if lhs is less than rhs, 0 if equal, or 1 if lhs greater than rhs */
    virtual int Compare(const T &lhs, const U &rhs) const = 0;

    /** So you can be deleted by this interface. */
    virtual ~IComparerDifferent(){}
};



/** A class to compare two objects of like types.
    Use to implement custom comparators for classes.
    In general, it is easiest to implement the less-than operator and take advantage of
    the DefaultComparer.
    \sa DefaultComparer
*/
template <class T> class IComparer :
        public IComparerDifferent<T, T>
{
public:
    /** So you can be deleted by this interface. */
    virtual ~IComparer(){}
};



/** The default comparer implements IComparer using the less-than operator to
    conduct comparisons.
*/
template <class T> class DefaultComparer :
        public IComparer<T>
{
public:
    /** The default compare function uses the less-than operator.
        It is easiest to implement the less-than operator and take advantage of
        the DefaultComparer.
    */
    virtual int Compare(const T&lhs, const T& rhs) const
    {
        int ret = 0;

        if(lhs < rhs)
            ret = -1;
        else if(rhs < lhs)
            ret = 1;

        return ret;
    }

    /** So you can be deleted by this interface */
    virtual ~DefaultComparer(){}
};


GUTIL_END_CORE_NAMESPACE

#endif // GUTIL_ICOMPARER_H
