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

#include "nodechain.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

node_link::node_link()
    :NextNode(0)
{}

node_link::~node_link()
{
    if(NextNode)
        delete NextNode;
}

bidirectional_node_link::bidirectional_node_link()
    :NextNode(0),
      PreviousNode(0)
{}

bidirectional_node_link::~bidirectional_node_link()
{
    if(NextNode)
        delete NextNode;
}



node_t::node_t()
    :Data(0)
{}

bidirectional_node_t::bidirectional_node_t()
    :Data(0)
{}





forward_node_iterator::forward_node_iterator(node_t *n, node_link *p)
    :current(n),
      parent(p)
{}

void forward_node_iterator::advance()
{
    if(current)
    {
        parent = current;
        current = current->NextNode;
    }
}

bool forward_node_iterator::operator == (const forward_node_iterator &o) const
{
    return current == o.current;
}

bool forward_node_iterator::operator != (const forward_node_iterator &o) const
{
    return current != o.current;
}


bidirectional_node_iterator::bidirectional_node_iterator(bidirectional_node_t *n)
    :current(n)
{}

void bidirectional_node_iterator::advance()
{
    if(current)
        current = current->NextNode;
}

void bidirectional_node_iterator::retreat()
{
    if(current)
        current = current->NextNode;
}

bool bidirectional_node_iterator::operator == (const bidirectional_node_iterator &o) const
{
    return current == o.current;
}

bool bidirectional_node_iterator::operator != (const bidirectional_node_iterator &o) const
{
    return current != o.current;
}
