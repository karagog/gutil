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

#include "ireadonlyobject.h"
GUTIL_USING_CORE_NAMESPACE(Interfaces);

IReadOnlyObject::IReadOnlyObject(bool readonly)
{
    _init_readonly_interface(readonly);
}

IReadOnlyObject::IReadOnlyObject(const Interfaces::IReadOnlyObject &other)
{
    _init_readonly_interface(other._my_readonly_bool);
}

void IReadOnlyObject::_init_readonly_interface(bool readonly)
{
    _my_readonly_bool = readonly;
    SetReadonlyBooleanReference(_my_readonly_bool);
}

std::string IReadOnlyObject::ReadonlyMessageIdentifier() const
{
    // This string is not too useful.  Derived classes should override to give more information
    return "Core::Interfaces::IReadonlyObject";
}

void IReadOnlyObject::SetReadonlyBooleanReference(bool &readonlybool)
{
    _readonly_bool_reference = &readonlybool;
}

void IReadOnlyObject::ClearReadonlyBooleanReference()
{
    SetReadonlyBooleanReference(_my_readonly_bool);
}

void IReadOnlyObject::TrackReadonlyObject(const Interfaces::IReadOnlyObject &other)
{
    SetReadonlyBooleanReference(*other._readonly_bool_reference);
}

void IReadOnlyObject::SetReadOnly(bool readonly)
{
    on_set_readonly(readonly);
    *_readonly_bool_reference = readonly;
}

void IReadOnlyObject::on_set_readonly(bool)
{

}

void IReadOnlyObject::FailIfReadOnly() const
        throw(ReadOnlyException<false>)
{
    if(IsReadOnly())
        THROW_NEW_GUTIL_EXCEPTION( ReadOnlyException );
}

bool IReadOnlyObject::IsReadOnly() const
{
    return *_readonly_bool_reference;
}
