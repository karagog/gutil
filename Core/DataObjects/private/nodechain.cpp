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
    :PreviousNode(0)
{}

bidirectional_node_link::~bidirectional_node_link()
{
    if(PreviousNode)
        delete PreviousNode;
}



node_t::node_t()
    :Data(0)
{}

bidirectional_node_t::bidirectional_node_t()
    :Data(0)
{}





forward_node_iterator::forward_node_iterator(node_t *n)
    :current(n)
{}

forward_node_iterator &forward_node_iterator::operator++()
{
    advance();
    return *this;
}

forward_node_iterator forward_node_iterator::operator++(int)
{
    forward_node_iterator ret(*this);
    advance();
    return ret;
}

void forward_node_iterator::advance()
{
    if(current)
    {
        if(current->NextNode)
            current = current->NextNode;
        else
            current = 0;
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

forward_node_iterator &forward_node_iterator::operator+=(int num)
{
    while(num-- > 0)
        advance();
    return *this;
}

forward_node_iterator forward_node_iterator::operator+(int num)
{
    forward_node_iterator ret(*this);
    while(num-- > 0)
        ret.advance();
    return ret;
}
