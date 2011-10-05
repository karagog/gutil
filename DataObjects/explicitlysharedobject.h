/*Copyright Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef EXPLICITLYSHAREDOBJECT_H
#define EXPLICITLYSHAREDOBJECT_H

#include "gutil.h"
#include <QExplicitlySharedDataPointer>

GUTIL_BEGIN_NAMESPACE( DataObjects );


template <class T>
class ExplicitlySharedObject
{
    template <class U> friend class ExplicitlySharedObject;

public:

    inline bool operator == (const ExplicitlySharedObject<T> &o) const{
        return _explicitly_shared_data == o._explicitly_shared_data;
    }

    inline bool operator != (const ExplicitlySharedObject<T> &o) const{
        return _explicitly_shared_data != o._explicitly_shared_data;
    }

    template <class U> inline bool operator == (const ExplicitlySharedObject<U> &o) const{
        return GetExplicitlySharedData() == (T *)o.GetExplicitlySharedData();
    }

    template <class U> inline bool operator != (const ExplicitlySharedObject<U> &o) const{
        return GetExplicitlySharedData() != (T *)o.GetExplicitlySharedData();
    }


protected:

    // Derived classes initialize this base with their own custom shared data object.
    //  This class will delete it on destruction
    inline explicit ExplicitlySharedObject(T *d)
        :_explicitly_shared_data(d)
    {}

    // You must call this copy constructor in your derived class'
    inline explicit ExplicitlySharedObject(const ExplicitlySharedObject<T> &o)
        :_explicitly_shared_data(o._explicitly_shared_data)
    {}

    inline T *GetExplicitlySharedData() const{
        return _explicitly_shared_data.data();
    }

    inline void SetExplicitlySharedData(T *d){
        _explicitly_shared_data = d;
    }

    // This function detaches the explicitly shared data and calls the copy constructor
    //  of the shared data object
    inline void Detach(){
        _explicitly_shared_data.detach();
    }


private:

    QExplicitlySharedDataPointer<T> _explicitly_shared_data;

};


GUTIL_END_NAMESPACE

#endif // EXPLICITLYSHAREDOBJECT_H
