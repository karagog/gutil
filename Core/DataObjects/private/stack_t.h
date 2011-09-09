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

#ifndef STACK_T_H
#define STACK_T_H

#include "nodechain.h"
#include "Core/Interfaces/ivoidwrappers.h"
#include <iterator>
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** My own implementation of a stack.  It uses an injected type wrapper which knows how to
    allocate/deallocate memory for a typed object, so we only manage void *'s.  That way almost
    all of the stack implementation is baked into the library.
*/
class stack_t :
        protected node_link
{
    GUTIL_DISABLE_COPY(stack_t);
public:

    /** How many items are on the stack.
        \note O(1)
    */
    inline long Count(){
        return m_count;
    }

    /** Empties the stack and clears all memory.
        \note O(N)
    */
    void Clear();

protected:

    /** The type-specific functions that we require. */
    class stack_type_wrapper :
            public Interfaces::IVoidCopyer,
            public Interfaces::IVoidDeleter
    {
    public:
        /** You will be deleted by this interface. */
        virtual ~stack_type_wrapper();
    };

    /** The type wrapper will be owned and deleted by the stack. */
    stack_t(stack_type_wrapper *);
    ~stack_t();

    /** Push an item on the stack
        \note O(1)
    */
    void push(const void *const);

    /** The item on top of the stack.
        \note O(1)
    */
    void *top();

    /** The item on top of the stack.
        \note O(1)
    */
    void const*top() const;

    /** The iterator is still valid after removal; it equals the next item on the stack.
        \note O(1), despite the fact that it's implemented as a singly-linked list.
        The iterator keeps track of the current's parent as you iterate through the stack,
        so that eliminates the need to find the parent (which is why the STL slist class takes O(N))
    */
    void remove(forward_node_iterator &);


private:

    stack_type_wrapper *data_wrapper;
    int m_count;

};


GUTIL_END_CORE_NAMESPACE;

#endif // STACK_T_H
