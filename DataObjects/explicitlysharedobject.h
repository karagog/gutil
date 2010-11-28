/*Copyright 2010 George Karagoulis

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

#include "gutil_macros.h"
#include "Custom/gshareddata.h"
#include "Custom/gshareddatapointer.h"
#include <QExplicitlySharedDataPointer>

GUTIL_BEGIN_NAMESPACE( DataObjects );


template <class T>
class ExplicitlySharedObject
{
protected:

    // Derived classes initialize this base with their own custom shared data object.
    //  This class will delete it on destruction
    ExplicitlySharedObject(T *d)
        :_explicitly_shared_data(d){}

    // You must call this copy constructor in your derived class'
    ExplicitlySharedObject(const ExplicitlySharedObject<T> &o)
        :_explicitly_shared_data(o._explicitly_shared_data){}

    virtual ~ExplicitlySharedObject(){
        // This is a template constraint; T must be a type derived from GSharedData
        if(T::IsDerivedFromGSharedData);
    }

    T *GetExplicitlySharedData() const{
        return _explicitly_shared_data.data();
    }

    void SetExplicitlySharedData(T *d){
        _explicitly_shared_data = d;
    }

    // This function detaches the explicitly shared data and calls the copy constructor
    //  of the shared data object
    void Detach(){
        SetExplicitlySharedData(new T(*GetExplicitlySharedData()));
    }


private:

    Custom::GSharedDataPointer<T> _explicitly_shared_data;

};


GUTIL_END_NAMESPACE

#endif // EXPLICITLYSHAREDOBJECT_H
