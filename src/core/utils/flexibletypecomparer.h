/*Copyright 2010-2015 George Karagoulis

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

#include <gutil/icomparer.h>
#include <gutil/globals.h>
NAMESPACE_GUTIL;


/** A class that provides a flexible, modifiable, compare method.
    Simply inject your own compare function into the constructor to modify the compare
    functionality.
*/
template<class T>class FlexibleTypeComparer :
        public IComparer<T>
{
    int (*compare)(const T &, const T &);

public:

    /** Constructs a type comparer with the default compare function (less-than operator). */
    FlexibleTypeComparer(){ _init(&GUtil::Compare<T>); }

    /** Constructs a type comparer with a compare function you supply. */
    FlexibleTypeComparer(int (*cmp)(const T &, const T &)){ _init(cmp); }

    /** This instance will take on the compare behavior of the given argument. */
    FlexibleTypeComparer<T> &operator = (const FlexibleTypeComparer<T> &ftc){
        this->~FlexibleTypeComparer<T>();
        new(this) FlexibleTypeComparer<T>(ftc);
        return *this;
    }

    /** This instance will take on the compare behavior of the given argument. */
    FlexibleTypeComparer<T> &operator = (int (*cmp)(const T &, const T &)){
        this->~FlexibleTypeComparer<T>();
        new(this) FlexibleTypeComparer<T>(cmp);
        return *this;
    }

    /** Virtual to allow deletion by the interface. */
    virtual ~FlexibleTypeComparer(){}

    /** Dereferences the function pointer and calls whichever compare function you gave it.
        Use this when you want to compare two objects of type T.

        This satisfies the IComparer interface.
    */
    int Compare(const T &lhs, const T &rhs) const{ return compare(lhs, rhs); }


private:

    void _init(int (*cmp)(const T &, const T &)){
        compare = cmp;
    }

};


template<class T>struct IsMovableType< FlexibleTypeComparer<T> >{ enum{ Value = 1 }; };


END_NAMESPACE_GUTIL;

#endif // GUTIL_FLEXIBLE_TYPE_COMPARER_H
