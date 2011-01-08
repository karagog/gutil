/*Copyright 2010 George Karagoul
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef IUPDATABLE_H
#define IUPDATABLE_H

#include "gutil_macros.h"

GUTIL_BEGIN_CORE_NAMESPACE( Interfaces );


class IUpdatable
{
public:

    inline IUpdatable(bool dirty = false)
        :_iupdatable_is_dirty(dirty),
        _update_counter(0),
        _backup_update_counter(0){}
    inline IUpdatable(const IUpdatable &o)
        :_iupdatable_is_dirty(o._iupdatable_is_dirty),
        _update_counter(0),
        _backup_update_counter(0){}
    virtual ~IUpdatable(){}

    virtual bool IsDirty() const{
        return _iupdatable_is_dirty;
    }

    inline bool IsClean() const{
        return !IsDirty();
    }

    void MakeDirty(){
        if(!_iupdatable_is_dirty)
            _iupdatable_is_dirty = true;
        _update_counter++;
        on_make_dirty();
    }

    // Derived classes should call this base version to reset dirty bit
    inline void CommitChanges(bool commit = true){
        commit_reject_changes(commit);
        _iupdatable_is_dirty = false;

        if(commit)
            _backup_update_counter = _update_counter;
        else
            _update_counter = _backup_update_counter;
    }

    inline void RejectChanges(){
        CommitChanges(false);
    }

    IUpdatable &operator =(const IUpdatable &o){
        _iupdatable_is_dirty = o._iupdatable_is_dirty;
        return *this;
    }

    inline long GetUpdateCounter() const{
        return _update_counter;
    }


protected:

    // Derived classes can act on these 'events'
    virtual void on_make_dirty(){}

    // True if committing, otherwise rejecting
    virtual void commit_reject_changes(bool){}


private:

    bool _iupdatable_is_dirty;

    long _update_counter;
    long _backup_update_counter;

};


GUTIL_END_CORE_NAMESPACE

#endif // IUPDATABLE_H
