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

bidirectional_node_link::bidirectional_node_link()
    :NextNode(0),
      PreviousNode(0)
{}

bidirectional_node_link::~bidirectional_node_link()
{
    if(NextNode)
        delete NextNode;
}



bidirectional_node_t::bidirectional_node_t()
    :Data(0)
{}






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
        current = current->PreviousNode;
}

bool bidirectional_node_iterator::operator == (const bidirectional_node_iterator &o) const
{
    return current == o.current;
}

bool bidirectional_node_iterator::operator != (const bidirectional_node_iterator &o) const
{
    return current != o.current;
}
