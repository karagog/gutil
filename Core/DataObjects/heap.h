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

#ifndef GUTIL_HEAP_H
#define GUTIL_HEAP_H

#include "gutil_macros.h"
#include "Core/DataObjects/vector.h"
#include "Core/Utils/flexibletypecomparer.h"
NAMESPACE_GUTIL1(DataObjects);


/** Implements a heap of objects.  You can modify the sorting behavior by injecting a new
    compare function.  In that way this is flexible enough to be a min-heap or max-heap.

    These are useful for implementing priority queues.
*/
template<class T>class Heap
{
public:

    /** Creates an empty heap with default compare (min-heap) */
    inline Heap(){}

    /** Creates an empty heap with your own custom compare function. */
    inline Heap(const Utils::FlexibleTypeComparer<T> &f)
        :compare(f)
    {}

    /** Inserts the item into the heap.
        \note O(log(N))
    */
    inline void Push(const T &i)
    {
        const GUINT32 indx( Count() );
        data.Insert(i, indx);
        _heapify_up();
    }

    /** Removes the item at the top of the heap.
        \note O(log(N))
    */
    inline void Pop()
    {
        if(Count() == 0) return;

        // First swap the top item with the lowest leaf
        const GUINT32 indx( Count() - 1 );
        if(indx > 0)
        {
            if(IsMovableType<T>::Value)
                gSwap(data.Data(), data + indx, sizeof(T));
            else
            {
                T mem(data[0]);
                data[0] = data[indx];
                data[indx] = mem;
            }
        }

        // Then remove the leaf node (which was just swapped from the top)
        data.RemoveAt(indx);

        // Then heapify the new top node downwards
        _heapify_down();
    }

    /** Returns the item at the top of the heap.
        \note O(1)
    */
    inline const T &Top() const{ return data[0]; }

    /** Returns the item at the top of the heap.
        \note Be sure not to modify the sorting key!
        \note O(1)
    */
    inline T &Top(){ return data[0]; }

    /** The number of items in the heap. */
    inline GUINT32 Count() const{ return data.Size(); }

    /** Returns a pointer to the start of the heap array. */
    inline const T *ConstData() const{ return data.ConstData(); }
    /** Returns a pointer to the start of the heap array.
        \warning Do not modify the sorting key for the heap, or the behavior
                    is undefined.
    */
    inline T *Data(){ return data.Data(); }


private:

    Vector<T> data;
    Utils::FlexibleTypeComparer<T> compare;

    void _heapify_up();
    void _heapify_down();

};



/** Declares a min-heap. */
template<class T>class MinHeap : public Heap<T>{};


/** Declares a max-heap, which is just a min heap with a backwards compare function. */
template<class T>class MaxHeap : public Heap<T>
{
public:
    inline MaxHeap() :Heap<T>(&maxheap_compare){}

private:
    static int maxheap_compare(const T &lhs, const T &rhs)
    {
        if(lhs < rhs)
            return 1;
        if(rhs < lhs)
            return -1;
        return 0;
    }
};




template<class T>void Heap<T>::_heapify_up()
{
    GUINT32 indx( Count() );
    GUINT32 parent_indx(indx >> 1);

    while(parent_indx != 0)
    {
        if(0 <= compare(data[indx - 1], data[parent_indx - 1]))
            break;

        if(IsMovableType<T>::Value)
            gSwap(data + (indx - 1), data + (parent_indx - 1), sizeof(T));
        else
        {
            T mem( data[indx - 1] );
            data[indx - 1] = data[parent_indx - 1];
            data[parent_indx - 1] = mem;
        }

        indx = parent_indx;
        parent_indx = indx >> 1;
    }
}

template<class T>void Heap<T>::_heapify_down()
{
    const GUINT32 indx( Count() + 1 );
    if(indx == 1)
        return;

    GUINT32 cur( 1 );
    int lvl_cnt(1);

    G_FOREVER
    {
        const GUINT32 lchild_indx( cur << 1 );
        const GUINT32 rchild_indx( lchild_indx + 1 );
        GUINT32 min_index;

        // If we're at a leaf node, then the heap condition is satisfied
        if(lchild_indx > Count())
            break;
        else if(rchild_indx > Count())
        {
            min_index = lchild_indx;
        }
        else
        {
            min_index = compare(data[lchild_indx - 1], data[rchild_indx - 1]) <= 0 ?
                        lchild_indx : rchild_indx;
        }

        // If I'm smaller than my smallest child, then the heap condition is satisfied
        if(compare(data[cur - 1], data[min_index - 1]) <= 0)
            break;

        // Or else I need to swap with my smallest child
        if(IsMovableType<T>::Value)
            gSwap(data + (cur - 1), data + (min_index - 1), sizeof(T));
        else
        {
            T mem(data[cur - 1]);
            data[cur - 1] = data[min_index - 1];
            data[min_index - 1] = mem;
        }

        ++lvl_cnt;
        cur = min_index;
    }
}


END_NAMESPACE_GUTIL1;


namespace GUtil
{

template<class T>struct IsMovableType< DataObjects::Heap<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::MinHeap<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::MaxHeap<T> >{ enum{ Value = 1 }; };

}


#endif // GUTIL_GUTIL_HEAP_H
