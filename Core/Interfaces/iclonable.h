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

#ifndef ICLONABLE_H
#define ICLONABLE_H

#include "gutil_macros.h"

NAMESPACE_GUTIL1( Interfaces );


// A class to define a clonable object interface.

// The template type must be the type of the derived class, otherwise the default
//  'CloneFrom' implementation won't work.  You can override it if you wish to fix
//  this behavior.

template <class T> class IClonable
{
public:

    // Clones this object to the provided object reference
    virtual T& CloneTo(T &) const = 0;

    virtual ~IClonable(){}

};


END_NAMESPACE_GUTIL1

#endif // ICLONABLE_H
