/*Copyright 2010-2012 George Karagoul
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_IUPDATABLE_H
#define GUTIL_IUPDATABLE_H

#include "gutil_macros.h"

NAMESPACE_GUTIL1( Interfaces );


class IUpdatable
{
    GUTIL_DISABLE_COPY(IUpdatable);
public:

    inline explicit IUpdatable(bool dirty = false)
        :m_dirty(dirty),
          m_update_cnt(0),
          m_backup_update_count(0)
    {}

    /** By default this returns the state of our internal boolean, but you can
        override to implement your own dirty state
    */
    virtual bool IsDirty() const{
        return m_dirty;
    }

    inline void MakeDirty(){
        m_update_cnt++;

        if(!m_dirty)
        {
            m_dirty = true;
            on_make_dirty();
        }
    }

    /** Commits changes to the object (implementation specific) and resets dirty state to "clean" */
    inline void CommitChanges(bool commit = true){
        commit_reject_changes(commit);
        m_dirty = false;

        if(commit)
            m_backup_update_count = m_update_cnt;
        else
            m_update_cnt = m_backup_update_count;
    }

    /** Rejects changes to the object (implementation specific) and resets dirty state to "clean" */
    inline void RejectChanges(){
        CommitChanges(false);
    }

    inline int UpdateCount() const{
        return m_update_cnt;
    }


protected:

    // Derived classes can act on these 'events'
    virtual void on_make_dirty(){}

    // True if committing, otherwise rejecting
    virtual void commit_reject_changes(bool){}


private:

    bool m_dirty;

    int m_update_cnt;
    int m_backup_update_count;

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_IUPDATABLE_H
