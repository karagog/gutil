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

#ifndef IEQUALITYCOMPARER_H
#define IEQUALITYCOMPARER_H

#include <gutil/macros.h>

NAMESPACE_GUTIL;


// A class to compare two objects of different types
template <class T, class U> class IEqualityComparerDifferent
{
public:

    virtual bool Equal(const T &lhs, const U &rhs) const = 0;

    virtual ~IEqualityComparerDifferent(){}
};



// A class to compare two objects of like types
template <class T> class IEqualityComparer
    : public IEqualityComparerDifferent<T, T>
{
public:

    virtual ~IEqualityComparer(){}
};




// Default requires a '==' operator
template <class T> class DefaultEqualityComparer
    : public IEqualityComparer<T>
{
public:

    virtual bool Equal(const T &lhs, const T &rhs) const{
        return lhs == rhs;
    }

    virtual ~DefaultEqualityComparer(){}
};


END_NAMESPACE_GUTIL;

#endif // IEQUALITYCOMPARER_H
