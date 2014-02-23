/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef IREADONLYOBJECT_H
#define IREADONLYOBJECT_H

#include "gutil_macros.h"
#include "gutil_exception.h"

NAMESPACE_GUTIL1( Interfaces );


/** Declares an interface that says you have a "Readonly" state.
*/
class IReadOnlyObject
{
public:

    /** Use this function to track a different readonly bool variable.
        This could be useful if you have a global boolean switch that may determine
        if items are readonly.
    */
    void SetReadonlyBooleanReference(bool &readonlybool){
        _readonly_bool_reference = &readonlybool;
    }

    /** If you had changed the readonly boolean reference, this will change it back
        to our own boolean in memory.
    */
    void ClearReadonlyBooleanReference(){
        SetReadonlyBooleanReference(m_readonly);
    }

    /** This object's readonly state will shadow the other's */
    void TrackReadonlyObject(const IReadOnlyObject &o){
        SetReadonlyBooleanReference(*o._readonly_bool_reference);
    }


    /** Returns true if we are currently in the readonly state */
    bool IsReadOnly() const{ return *_readonly_bool_reference; }

    /** Marks us "Readonly" */
    void SetReadOnly(bool readonly = true){
        on_set_readonly(readonly);
        *_readonly_bool_reference = readonly;
    }

    /** Throws an exception if we are currently readonly. */
    void FailIfReadOnly() const{
        if(IsReadOnly())
            THROW_NEW_GUTIL_EXCEPTION2( ReadOnlyException, ReadonlyMessageIdentifier() );
    }


protected:

    IReadOnlyObject(bool readonly = false){
        _init_readonly_interface(readonly);
    }

    IReadOnlyObject(const IReadOnlyObject &other){
        _init_readonly_interface(other.IsReadOnly());
    }

    /** You can throw an exception to halt the readonly setting */
    virtual void on_set_readonly(bool){}

    /** Derived classes return a useful string to identify the object
        which threw a ReadOnlyException
    */
    virtual const char *ReadonlyMessageIdentifier() const{ return "GUtil::Interfaces::IReadonlyObject"; }

private:

    bool *_readonly_bool_reference;
    bool m_readonly;

    void _init_readonly_interface(bool readonly){
        m_readonly = readonly;
        SetReadonlyBooleanReference(m_readonly);
    }

};


END_NAMESPACE_GUTIL1;

#endif // IREADONLYOBJECT_H
