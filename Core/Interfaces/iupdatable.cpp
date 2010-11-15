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

#include "iupdatable.h"
using namespace GUtil;

Core::Interfaces::IUpdatable::IUpdatable(bool dirty)
{
    _iupdatable_is_dirty = dirty;
}

Core::Interfaces::IUpdatable::IUpdatable(const IUpdatable &o)
{
    *this = o;
}

Core::Interfaces::IUpdatable::~IUpdatable(){}

bool Core::Interfaces::IUpdatable::IsDirty() const
{
    return _iupdatable_is_dirty;
}

void Core::Interfaces::IUpdatable::SetDirty(bool d)
{
    if(_iupdatable_is_dirty != d)
        on_set_dirty(_iupdatable_is_dirty = d);
}

void Core::Interfaces::IUpdatable::on_set_dirty(bool){}

void Core::Interfaces::IUpdatable::commit_reject_changes(bool){}

Core::Interfaces::IUpdatable &Core::Interfaces::IUpdatable::operator =
        (const Core::Interfaces::IUpdatable &o)
{
    _iupdatable_is_dirty = o._iupdatable_is_dirty;
    return *this;
}

void Core::Interfaces::IUpdatable::CommitChanges()
{
    commit_reject_changes(true);
    SetDirty(false);
}

void Core::Interfaces::IUpdatable::RejectChanges()
{
    commit_reject_changes(false);
    SetDirty(false);
}
