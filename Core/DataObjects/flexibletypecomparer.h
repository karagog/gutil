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

#ifndef GUTIL_FLEXIBLE_TYPE_COMPARER_H
#define GUTIL_FLEXIBLE_TYPE_COMPARER_H

#include "Core/Interfaces/icomparer.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


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
        compare = &default_compare;
    }

    /** Constructs a type comparer with a compare function you supply. */
    inline FlexibleTypeComparer(int (*cmp)(const T &, const T &)){
        compare = cmp;
    }


    /** Dereferences the function pointer and calls whichever compare function you gave it.
        Use this when you want to compare two objects of type T.

        This satisfies the IComparer interface.
    */
    int Compare(const T &lhs, const T &rhs) const{ return compare(lhs, rhs); }

    /** Makes this a "function object", so you can either use this or Compare() to compare values. */
    inline int operator () (const T &lhs, const T &rhs) const{
        return FlexibleTypeComparer<T>::Compare(lhs, rhs);
    }


private:
    int (*compare)(const T &, const T &);

    /** The default compare function is the less-than operator.

        I believe this is not included unless you use the default constructor, so it's no matter
        if your class doesn't implement a less-than operator, you can implement whatever comparator you want.
    */
    static int default_compare(const T &lhs, const T &rhs){
        if(lhs < rhs)
            return -1;
        if(rhs < lhs)
            return 1;
        return 0;
    }
};


GUTIL_END_CORE_NAMESPACE;


namespace GUtil
{

template<class T>struct IsMovableType< Core::DataObjects::FlexibleTypeComparer<T> >{ enum{ Value = 1 }; };

}

#endif // GUTIL_FLEXIBLE_TYPE_COMPARER_H
