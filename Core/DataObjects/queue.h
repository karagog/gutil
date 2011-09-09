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

#ifndef QUEUE_H
#define QUEUE_H

#include "Core/DataObjects/private/queue_p.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


template<class T>class Queue :
        public queue_p
{
public:
    class iterator :
            public bidirectional_node_iterator
    {
    public:
        /** Advances the iterator */
        iterator &operator++();

        /** Advances the iterator */
        iterator operator++(int);

        /** Advances the iterator the specified number of items */
        iterator &operator+=(int);

        /** Returns a copy of the iterator advanced the specified number of times. */
        iterator operator+(int);

        /** Retreats the iterator */
        iterator &operator--();

        /** Retreats the iterator */
        iterator operator--(int);

        /** Retreats the iterator the specified number of items */
        iterator &operator-=(int);

        /** Returns a copy of the iterator retreated the specified number of times. */
        iterator operator-(int);
    };
};


GUTIL_END_CORE_NAMESPACE;

#endif // QUEUE_H
