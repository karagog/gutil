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

#ifndef GUTIL_DLIST_P_H
#define GUTIL_DLIST_P_H

#include "nodechain.h"
#include "Core/Interfaces/ivoidwrappers.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a doubly-linked list.

    Doubly-linked lists are optimized for lots of insertions/removals, but really suck at
    accessing the list randomly.  It is best to use an iterator.
*/
class dlist_p :
        protected bidirectional_node_link
{
    GUTIL_DISABLE_COPY(dlist_p);
public:

    long Size() const;
    inline long Length() const{ return Size(); }
    inline long Count() const{ return Size(); }

    void Clear();


protected:

    class type_wrapper :
            public Interfaces::IVoidCopyer,
            public Interfaces::IVoidDeleter
    {
    public:
        virtual ~type_wrapper(){}
    };

    dlist_p(type_wrapper *);
    inline ~dlist_p(){ Clear(); delete data_wrapper; }

    void push_front(const void *const);
    void push_back(const void *const);
    void pop_front();
    void pop_back();

    void insert(const void *const, bidirectional_node_iterator iter);
    void remove(bidirectional_node_iterator);


private:

    type_wrapper *data_wrapper;
    long m_size;

};


GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_DLIST_P_H
