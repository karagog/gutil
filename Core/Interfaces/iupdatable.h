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
        :_iupdatable_is_dirty(dirty){}
    inline IUpdatable(const IUpdatable &o)
        :_iupdatable_is_dirty(o._iupdatable_is_dirty){}
    virtual ~IUpdatable(){}

    virtual bool IsDirty() const{
        return _iupdatable_is_dirty;
    }

    virtual void SetDirty(bool d){
        on_set_dirty(_iupdatable_is_dirty = d);
    }

    // Derived classes should call this base version to reset dirty bit
    inline void CommitChanges(){
        commit_reject_changes(true);
        SetDirty(false);
    }

    inline void RejectChanges(){
        commit_reject_changes(false);
        SetDirty(false);
    }

    IUpdatable &operator =(const IUpdatable &o){
        _iupdatable_is_dirty = o._iupdatable_is_dirty;
        return *this;
    }


protected:

    // Derived classes can act on these 'events'
    virtual void on_set_dirty(bool){}

    // True if committing, otherwise rejecting
    virtual void commit_reject_changes(bool commit){}


private:

    bool _iupdatable_is_dirty;

};


GUTIL_END_CORE_NAMESPACE

#endif // IUPDATABLE_H
