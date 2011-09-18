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

#include "dlist_p.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

dlist_p::dlist_p(dlist_p::type_wrapper *tw)
    :data_wrapper(tw),
      m_size(0)
{}

void dlist_p::Clear()
{
    bidirectional_node_iterator iter(NextNode);
    while(iter) remove(iter);
}

void dlist_p::insert(const void *const v, bidirectional_node_iterator &iter)
{
    bidirectional_node_t *new_node(new bidirectional_node_t);
    new_node->Data = data_wrapper->CopyVoid(v);

    new_node->NextNode = iter.current;

    if(iter.current)
    {
        new_node->PreviousNode = iter.current->PreviousNode;
        if(iter.current == NextNode)
            NextNode = new_node;
        else
            iter.current->PreviousNode->NextNode = new_node;
        iter.current->PreviousNode = new_node;
    }
    else
    {
        // Pushing onto the end of the list.
        if(PreviousNode)
        {
            // The list already has items in it.
            PreviousNode->NextNode = new_node;
            new_node->PreviousNode = PreviousNode;
            PreviousNode = new_node;
        }
        else
        {
            // Pushing onto empty list
            NextNode = new_node;
            PreviousNode = new_node;
        }
    }

    m_size++;
}

void dlist_p::remove(bidirectional_node_iterator &iter)
{
    bidirectional_node_t *n(iter.current);
    if(n)
    {
        if(NextNode == n)
            NextNode = n->NextNode;
        else
            n->PreviousNode->NextNode = n->NextNode;

        // So the iterator is still valid after the removal.
        iter.current = n->NextNode;

        if(n->NextNode)
            n->NextNode->PreviousNode = n->PreviousNode;
        else
            PreviousNode = n->PreviousNode;

        data_wrapper->DeleteVoid(n->Data);
        n->NextNode = 0;
        delete n;

        m_size--;
    }
}
