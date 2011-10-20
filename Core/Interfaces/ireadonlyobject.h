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

#ifndef IREADONLYOBJECT_H
#define IREADONLYOBJECT_H

#include "gutil.h"
#include "Core/exception.h"
#include <string>

GUTIL_BEGIN_CORE_NAMESPACE( Interfaces );


class IReadOnlyObject
{
public:

    // Use this function to track a global readonly bool variable
    virtual void SetReadonlyBooleanReference(bool &readonlybool);
    void ClearReadonlyBooleanReference();

    // Will set this object's boolean to shadow the other's
    void TrackReadonlyObject(const IReadOnlyObject &);

    bool IsReadOnly() const;

    void SetReadOnly(bool readonly = true);

    void FailIfReadOnly() const
            throw(GUtil::Core::ReadOnlyException<false>);

protected:
    IReadOnlyObject(bool readonly = false);
    IReadOnlyObject(const IReadOnlyObject &other);

    // You can throw an exception to halt the readonly setting
    virtual void on_set_readonly(bool);

    // Derived classes return a useful string to identify the object
    //  which threw a ReadOnlyException
    virtual std::string ReadonlyMessageIdentifier() const;

private:
    void _init_readonly_interface(bool readonly);

    bool *_readonly_bool_reference;
    bool _my_readonly_bool;
};


GUTIL_END_CORE_NAMESPACE

#endif // IREADONLYOBJECT_H
