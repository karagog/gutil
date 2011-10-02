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

#ifndef HEAP_H
#define HEAP_H

#include "gutil_macros.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a heap of objects.  You can modify the sorting behavior by injecting a new
    compare function.  In that way this is flexible enough to be a min-heap or max-heap.

    These are useful for implementing priority queues.
*/
template<class T>class Heap
{
    /** A node of the heap's binary tree structure. */
    class node
    {
    public:
        inline node(node *l, node *r, const T &d) :LChild(l), RChild(r), Data(d) {}

        node *LChild;
        node *RChild;
        T Data;
    };

public:

    inline Heap(){}

    void Insert(const T &){}
    void Pop(){}

    /** Returns the item at the top of the heap. */
    const T &Top() const{}

    /** Returns the item at the top of the heap.
        \note Be sure not to modify the sorting key!
    */
    T &Top(){}

};


GUTIL_END_CORE_NAMESPACE;

#endif // HEAP_H
