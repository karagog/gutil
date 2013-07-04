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

#ifndef GUTIL_FLEXIBLE_TYPE_COMPARER_H
#define GUTIL_FLEXIBLE_TYPE_COMPARER_H

#include "gutil_icomparer.h"
#include "gutil_globals.h"
NAMESPACE_GUTIL1(Utils)


/** A class that provides a flexible, modifiable, compare method.
    Simply inject your own compare function into the constructor to modify the compare
    functionality
*/
template<class T>class FlexibleTypeComparer :
        public Interfaces::IComparer<T>
{
public:
    /** Constructs a type comparer with the default compare function (less-than operator). */
    inline FlexibleTypeComparer(){
        compare = &CompareDefault;
    }

    /** Constructs a type comparer with a compare function you supply. */
    inline FlexibleTypeComparer(int (*cmp)(const T &, const T &)){
        compare = cmp;
    }


    /** Dereferences the function pointer and calls whichever compare function you gave it.
        Use this when you want to compare two objects of type T.

        This satisfies the IComparer interface.
    */
    virtual int Compare(const T &lhs, const T &rhs) const{ return compare(lhs, rhs); }

    /** The default compare function uses the less-than operator.

        This static function is made publicly available, in case you want to use the
        default compare without consuming the class itself
    */
    static int CompareDefault(const T &lhs, const T &rhs){
        if(lhs < rhs)
            return -1;
        if(rhs < lhs)
            return 1;
        return 0;
    }


private:

    int (*compare)(const T &, const T &);

};


END_NAMESPACE_GUTIL1



NAMESPACE_GUTIL

/** A convenience class that compares two objects with their less-than operator. */
template<class T>
class DefaultComparer : public Utils::FlexibleTypeComparer<T>{};

template<class T>struct IsMovableType< Utils::FlexibleTypeComparer<T> >{ enum{ Value = 1 }; };

END_NAMESPACE_GUTIL

#endif // GUTIL_FLEXIBLE_TYPE_COMPARER_H
