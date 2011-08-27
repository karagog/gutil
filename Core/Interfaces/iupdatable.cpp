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

#include "iupdatable.h"
GUTIL_USING_CORE_NAMESPACE(Interfaces);

IUpdatable::IUpdatable(bool dirty)
    :_iupdatable_is_dirty(dirty),
      _update_counter(0),
      _backup_update_counter(0)
{

}

IUpdatable::IUpdatable(const IUpdatable &o)
    :_iupdatable_is_dirty(o._iupdatable_is_dirty),
      _update_counter(0),
      _backup_update_counter(0)
{

}

void IUpdatable::MakeDirty()
{
    _update_counter++;

    if(!_iupdatable_is_dirty)
    {
        _iupdatable_is_dirty = true;
        on_make_dirty();
    }
}

bool IUpdatable::IsDirty() const
{
    return _iupdatable_is_dirty;
}

void IUpdatable::CommitChanges(bool commit)
{
    commit_reject_changes(commit);
    _iupdatable_is_dirty = false;

    if(commit)
        _backup_update_counter = _update_counter;
    else
        _update_counter = _backup_update_counter;
}

void IUpdatable::RejectChanges()
{
    CommitChanges(false);
}

void IUpdatable::on_make_dirty()
{

}

void IUpdatable::commit_reject_changes(bool)
{

}

IUpdatable &IUpdatable::operator =(const IUpdatable &o)
{
    _iupdatable_is_dirty = o._iupdatable_is_dirty;
    return *this;
}

long IUpdatable::GetUpdateCounter() const
{
    return _update_counter;
}
