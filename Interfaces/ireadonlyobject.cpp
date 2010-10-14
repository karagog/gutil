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

#include "ireadonlyobject.h"
using namespace GUtil;

Interfaces::IReadOnlyObject::IReadOnlyObject(bool readonly)
{
    _init(readonly);
}

Interfaces::IReadOnlyObject::IReadOnlyObject(const Interfaces::IReadOnlyObject &other)
{
    _init(other._my_readonly_bool);

    // If the other guy is referencing a global boolean, then we reference it too
    if(other._readonly_bool_reference != &other._my_readonly_bool)
        SetReadonlyBooleanReference(*other._readonly_bool_reference);
}

void Interfaces::IReadOnlyObject::_init(bool readonly)
{
    _my_readonly_bool = readonly;
    SetReadonlyBooleanReference(_my_readonly_bool);
}

void Interfaces::IReadOnlyObject::SetReadonlyBooleanReference(bool &readonlybool)
{
    _readonly_bool_reference = &readonlybool;
}

void Interfaces::IReadOnlyObject::ClearReadonlyBooleanReference()
{
    SetReadonlyBooleanReference(_my_readonly_bool);
}

void Interfaces::IReadOnlyObject::TrackReadonlyObject(const Interfaces::IReadOnlyObject &other)
{
    SetReadonlyBooleanReference(*other._readonly_bool_reference);
}

bool Interfaces::IReadOnlyObject::IsReadOnly() const
{
    return *_readonly_bool_reference;
}

void Interfaces::IReadOnlyObject::SetReadOnly(bool readonly)
{
    *_readonly_bool_reference = readonly;
}

void Interfaces::IReadOnlyObject::FailIfReadOnly() const throw(GUtil::Core::ReadOnlyException)
{
    if(IsReadOnly())
        throw Core::ReadOnlyException(ReadonlyMessageIdentifier());
}
