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

#ifndef GUTIL_SLIST_P_H
#define GUTIL_SLIST_P_H

#include "nodechain.h"
#include "Core/Interfaces/ivoidwrappers.h"
#include <iterator>
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** My own implementation of a stack.  It uses an injected type wrapper which knows how to
    allocate/deallocate memory for a typed object, so we only manage void *'s.  That way almost
    all of the stack implementation is baked into the library.
*/
class slist_p :
        protected node_link
{
    GUTIL_DISABLE_COPY(slist_p);
public:

    /** Is the slist empty? */
    inline bool IsEmpty() const{
        return !_count();
    }

    /** Empties the slist and clears all memory.
        \note O(N)
    */
    void Clear();

protected:

    /** How many items are in the slist
        \note O(1)
    */
    inline long _count() const{ return m_count; }

    /** A reference to the last item in the list, or to myself if there are no items. */
    node_link *m_end;

    /** The type-specific functions that we require. */
    class type_wrapper :
            public Interfaces::IVoidCopyer,
            public Interfaces::IVoidDeleter
    {
    public:
        /** You will be deleted by this interface. */
        virtual ~type_wrapper();
    };

    /** The type wrapper will be owned and deleted by the stack. */
    slist_p(type_wrapper *);
    ~slist_p();

    /** The iterator is still valid after insertion.
        It simply points to the same element, which has been shifted one place in the list.
        \note O(1)
    */
    void insert(void const *, forward_node_iterator &);

    /** The iterator is still valid after removal; it equals the next item on the stack.
        \note O(1), despite the fact that it's implemented as a singly-linked list.
        The iterator keeps track of the current's parent as you iterate through the stack,
        so that eliminates the need to find the parent (which is why the STL slist class takes O(N))
    */
    void remove(forward_node_iterator &);


private:

    type_wrapper *data_wrapper;
    int m_count;

};


GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_SLIST_P_H
