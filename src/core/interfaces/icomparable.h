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

#ifndef ICOMPARABLE_H
#define ICOMPARABLE_H

#include <gutil/macros.h>

NAMESPACE_GUTIL;


template <class T> class IComparable
{
public:

    // Return negative if this is less than the other, 0 if equal
    virtual int Compare(const T &) const = 0;


    // These functions are designed for convenience
    virtual bool operator <(const T &o) const{
        return Compare(o) < 0;
    }

    virtual bool operator >(const T &o) const{
        return Compare(o) > 0;
    }

    virtual bool operator ==(const T &o) const{
        return Compare(o) == 0;
    }

    virtual bool operator !=(const T &o) const{
        return Compare != 0;
    }

    virtual ~IComparable(){}
};


END_NAMESPACE_GUTIL;

#endif // ICOMPARABLE_H
