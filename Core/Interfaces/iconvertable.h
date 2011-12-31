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

#ifndef ICONVERTABLE_H
#define ICONVERTABLE_H

#include "gutil_macros.h"

NAMESPACE_GUTIL1( Interfaces );


// A class to define conversions between types

template <class T> class IConvertable
{
public:

    virtual T &ConvertTo(T &) = 0;
    virtual T &ConvertFrom(T &) = 0;

    virtual ~IConvertable(){}

};


END_NAMESPACE_GUTIL1

#endif // ICONVERTABLE_H
