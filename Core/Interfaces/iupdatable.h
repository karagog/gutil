/*Copyright 2011 George Karagoul
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

    IUpdatable(bool dirty = false);
    IUpdatable(const IUpdatable &o);

    virtual bool IsDirty() const;

    void MakeDirty();

    // Derived classes should call this base version to reset dirty bit
    void CommitChanges(bool commit = true);

    void RejectChanges();

    IUpdatable &operator =(const IUpdatable &o);

    long GetUpdateCounter() const;


protected:

    // Derived classes can act on these 'events'
    virtual void on_make_dirty();

    // True if committing, otherwise rejecting
    virtual void commit_reject_changes(bool);


private:

    bool _iupdatable_is_dirty;

    long _update_counter;
    long _backup_update_counter;

};


GUTIL_END_CORE_NAMESPACE

#endif // IUPDATABLE_H
