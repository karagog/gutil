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


/** My own implementation of a stack. */
class stack_t :
        protected node_link
{
    GUTIL_DISABLE_COPY(stack_t);
public:

    /** Pop an item from the stack.
        \note O(1)
    */
    void Pop();

    /** How many items are on the stack.
        \note O(1)
    */
    inline long Count(){
        return m_count;
    }

    void Clear();

protected:

    class stack_type_wrapper :
            public Interfaces::IVoidCopyer,
            public Interfaces::IVoidDeleter
    {
    public:
        virtual ~stack_type_wrapper();
    };

    stack_t(stack_type_wrapper *);

    void push(const void *const);
    void *top();
    const void *top() const;

    /** The iterator is still valid after removal; it equals the next item on the stack.
        \note O(N)
    */
    void remove(forward_node_iterator &);


private:

    stack_type_wrapper *data_wrapper;
    int m_count;

};


GUTIL_END_CORE_NAMESPACE;

#endif // STACK_T_H
