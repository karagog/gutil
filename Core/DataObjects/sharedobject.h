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

#ifndef GUTIL_SHAREDOBJECT_H
#define GUTIL_SHAREDOBJECT_H

#include "Core/Utils/smartpointer.h"

NAMESPACE_GUTIL1(DataObjects);


/** Defines an explicitly shared object, whose data is shared between class
    instances.  To conduct a full deep copy, call Detach().
*/
template <class T>class SharedObject
{
public:

    inline bool operator == (const SharedObject<T> &o) const{
        return _shared_data == o._shared_data;
    }

    inline bool operator != (const SharedObject<T> &o) const{
        return _shared_data != o._shared_data;
    }

    // This function detaches the explicitly shared data and calls the copy constructor
    //  of the shared data object
    inline void Detach(){
        _shared_data.Detach();
    }


protected:

    /** Derived classes initialize this base with their own custom shared data object.
        This class will delete it on destruction
    */
    inline explicit SharedObject(T *d)
        :_shared_data(d)
    {}

    /** You must call this copy constructor in your derived class */
    inline explicit SharedObject(const SharedObject<T> &o)
        :_shared_data(o._shared_data)
    {}

    inline T *GetSharedData() const{
        return _shared_data.data();
    }

    inline void SetSharedData(T *d){
        _shared_data = d;
    }


private:

    GUtil::Utils::SharedSmartPointer<T> _shared_data;

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_SHAREDOBJECT_H
