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

#ifndef GUTIL_ICOMPARER_H
#define GUTIL_ICOMPARER_H

#include <gutil/macros.h>
NAMESPACE_GUTIL;


/** The default GUtil compare function compares two items of potentially different types.
 *  You can provide your own specialized template to override this behavior.
 *
 *  \returns -1 if lhs is less than rhs, 0 if equal, or 1 if lhs greater than rhs
*/
template<class T, class U = T>
int Compare(const T &lhs, const U &rhs){
    if(lhs < rhs) return -1;
    else if(rhs < lhs) return 1;
    return 0;
}


/** An abstract interface class to compare two objects of different types */
template <class T, class U> class IComparerDifferent
{
public:

    /** Return -1 if lhs is less than rhs, 0 if equal, or 1 if lhs greater than rhs */
    virtual int Compare(const T &lhs, const U &rhs) const{
        return GUtil::Compare(lhs, rhs);
    }

    /** Convenience parenthesis operator that allows you to treat this like a function object. */
    int operator ()(const T &lhs, const U &rhs) const{
        return Compare(lhs, rhs);
    }

    /** So you can be deleted by this interface. */
    virtual ~IComparerDifferent(){}
};



/** A class to compare two objects of like types.
    Use to implement custom comparators for classes.
    In general, it is easiest to implement the less-than operator and take advantage of
    the DefaultComparer.
*/
template <class T> class IComparer :
        public IComparerDifferent<T, T>
{
public:
    virtual ~IComparer(){}
};


END_NAMESPACE_GUTIL;


#endif // GUTIL_ICOMPARER_H
