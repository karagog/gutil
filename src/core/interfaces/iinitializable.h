/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_IINITIALIZABLE_H
#define GUTIL_IINITIALIZABLE_H

#include "gutil_macros.h"

NAMESPACE_GUTIL;


/** Declares an initializable interface */
class IInitializable
{
public:

    /** Initializes the class. */
    virtual void Initialize() = 0;
    
    /** Uninitializes the class. */
    virtual void Uninitialize() = 0;
    
    /** Returns true if the class has been initialized. */
    virtual bool IsInitialized() = 0;

};


END_NAMESPACE_GUTIL1

#endif // GUTIL_IINITIALIZABLE_H
