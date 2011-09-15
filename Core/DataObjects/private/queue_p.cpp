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

#include "queue_p.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

queue_p::queue_p(queue_p::queue_type_wrapper *tw)
    :data_wrapper(tw),
      m_size(0)
{}

void queue_p::Clear()
{
    while(Count() > 0)
        pop_front();
}

void queue_p::insert(const void *const v, bidirectional_node_iterator iter)
{
    bidirectional_node_t *new_node(new bidirectional_node_t);

    new_node->NextNode = iter.current;

    if(!iter.current)
        iter.current = PreviousNode;

    new_node->PreviousNode = iter.current->PreviousNode;
    iter.current->PreviousNode = new_node;

    if(iter.current == NextNode)
    {
        NextNode->PreviousNode = new_node;
        NextNode = new_node;
    }

    new_node->Data = data_wrapper->CopyVoid(v);
    m_size++;
}

void queue_p::remove(bidirectional_node_iterator iter)
{
    bidirectional_node_t *n(iter.current);
    if(n)
    {
        if(n->PreviousNode)
            n->PreviousNode->NextNode = n->NextNode;
        if(n->NextNode)
            n->NextNode->PreviousNode = n->PreviousNode;

        data_wrapper->DeleteVoid(n->Data);
        n->NextNode = 0;
        delete n;

        m_size--;
    }
}

void queue_p::push_front(const void *const v)
{
    insert(v, bidirectional_node_iterator(NextNode));
}

void queue_p::push_back(const void *const v)
{
    insert(v, bidirectional_node_iterator(0));
}

void queue_p::pop_front()
{
    remove(NextNode);
}

void queue_p::pop_back()
{
    remove(PreviousNode);
}

long queue_p::Size() const
{
    return m_size;
}
