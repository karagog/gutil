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

#include "gutil.h"
#include "Core/DataObjects/vector.h"
#include "Core/DataObjects/flexibletypecomparer.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a heap of objects.  You can modify the sorting behavior by injecting a new
    compare function.  In that way this is flexible enough to be a min-heap or max-heap.

    These are useful for implementing priority queues.
*/
template<class T>class Heap
{
public:

    inline Heap(){}
    inline Heap(const FlexibleTypeComparer<T> &f)
        :compare(f)
    {}

    void Insert(const T &i)
    {
        data.Append(i);
        _heapify( data.Length() - 1 );
    }
    void Pop(){}

    /** Returns the item at the top of the heap. */
    inline const T &Top() const{ return data[1]; }

    /** Returns the item at the top of the heap.
        \note Be sure not to modify the sorting key!
    */
    inline T &Top(){ return data[1]; }


private:

    SimpleVector<T> data;
    FlexibleTypeComparer<T> compare;

};


GUTIL_END_CORE_NAMESPACE;

#endif // HEAP_H
