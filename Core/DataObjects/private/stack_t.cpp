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

#include "stack_t.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

stack_t::stack_t(stack_type_wrapper *stw)
    :data_wrapper(stw),
      m_count(0)
{}

stack_t::stack_type_wrapper::~stack_type_wrapper(){}

void stack_t::push(const void *const v)
{
    on_push(v);

    node_t *new_node( new node_t );
    new_node->NextNode = NextNode;
    NextNode = new_node;
    new_node->Data = data_wrapper->CopyVoid(v);
    m_count++;
}

const void *const stack_t::top() const
{
    return NextNode ? NextNode->Data : 0;
}

void *stack_t::top()
{
    return NextNode ? NextNode->Data : 0;
}

void stack_t::Pop()
{
    if(NextNode)
    {
        on_pop(NextNode->Data);

        node_t *t( NextNode );
        NextNode = NextNode->NextNode;

        data_wrapper->DeleteVoid(t->Data);
        t->NextNode = 0;
        delete t;

        m_count--;
    }
}

void stack_t::on_push(const void *const)
{

}

void stack_t::on_pop(const void *const)
{

}

void stack_t::remove(forward_node_iterator &iter)
{
    if(!iter.current)
        return;

    node_link *parent_link(0);
    if(NextNode == iter.current)
    {
        parent_link = static_cast<node_link *>(this);
    }
    else
    {
        // Need to find the parent of the iterator, which is why it takes O(N)
        node_t *cur(NextNode);
        while(cur)
        {
            if(cur->NextNode == iter.current)
            {
                parent_link = static_cast<node_link *>(cur);
                break;
            }
            cur = cur->NextNode;
        }
    }

    parent_link->NextNode = iter.current->NextNode;
    data_wrapper->DeleteVoid(iter.current->Data);
    iter.current->NextNode = 0;
    delete iter.current;

    iter.current = parent_link->NextNode;
    m_count--;
}
