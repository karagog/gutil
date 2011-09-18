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

#ifndef LIST_H
#define LIST_H

#include "private/nodechain.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** A list of items, implemented as a memory array.

    The memory for the list is separated into pages, where each successive page is twice as large
    as the previous one.  This has an advantage over Vector's memory management, because
    there are no large memory reallocations when you run out.  In this case we just allocate a new
    page that effectively doubles the capacity of the list.

    It implements the Deque, Queue and Stack interfaces.

    \sa Vector, Dlist, SList
*/
template<class T>class List
{
public:
    List()
        :
    {

    }

private:

    bidirectional_node_t *m_front;
    bidirectional_node_t *m_back;

};


GUTIL_END_CORE_NAMESPACE;

#endif // LIST_H
