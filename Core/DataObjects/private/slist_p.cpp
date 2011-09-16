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

#include "slist_p.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

slist_p::slist_p(slist_pype_wrapper *stw)
    :data_wrapper(stw),
      m_count(0)
{}

slist_p::~slist_p()
{
    Clear();
    delete data_wrapper;
}

slist_p::slist_pype_wrapper::~slist_pype_wrapper(){}

void slist_p::push(const void *const v)
{
    node_t *new_node( new node_t );
    new_node->NextNode = NextNode;
    NextNode = new_node;
    new_node->Data = data_wrapper->CopyVoid(v);
    m_count++;
}

void *slist_p::front()
{
    return NextNode ? NextNode->Data : 0;
}

void const*slist_p::front() const
{
    return NextNode ? NextNode->Data : 0;
}

void slist_p::Clear()
{
    forward_node_iterator top_of_stack(NextNode, this);
    while(top_of_stack)
        remove(top_of_stack);
}

void slist_p::remove(forward_node_iterator &iter)
{
    if(!iter.current)
        return;

    data_wrapper->DeleteVoid(iter.current->Data);
    node_t *n( iter.current->NextNode );
    iter.current->NextNode = 0;
    delete iter.current;

    iter.current = n;
    iter.parent->NextNode = n;
    m_count--;
}
