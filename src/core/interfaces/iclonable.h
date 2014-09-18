/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_ICLONABLE_H
#define GUTIL_ICLONABLE_H

#include "gutil_macros.h"

NAMESPACE_GUTIL;


/** A class to define a clonable object interface. */
class IClonable
{
public:

    /** Returns a copy of this object. You can dynamic cast
     *  the IClonable to test what type it is, assuming you
     *  have run time type information.
    */
    virtual IClonable *Clone() const noexcept = 0;

    /** You can be deleted by this interface. */
    virtual ~IClonable(){}

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_ICLONABLE_H
