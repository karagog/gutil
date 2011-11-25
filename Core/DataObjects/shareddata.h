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

#ifndef SHAREDDATA_H
#define SHAREDDATA_H

#include "gutil_macros.h"
#include "Core/Utils/atomic.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Inherit from this class to declare your class able to be used by the shared data
    pointer class.

    It keeps track of references to your class with atomic increment
    decrement classes.

    All functions are protected, because it is not meant to be used by the outside
    world; only by the shared data pointer.
*/
class SharedData
{
    Utils::AtomicInt m_references;
    GUTIL_DISABLE_COPY(SharedData)
    template<class T>friend class SharedSmartPointer;
protected:

    inline SharedData(){}

    inline bool AddReference(){ return m_references.Increment(); }
    inline bool RemoveReference(){ return m_references.Decrement(); }

    inline GINT32 ReferenceCount() const{ return m_references; }

};


GUTIL_END_CORE_NAMESPACE;

#endif // SHAREDDATA_H
