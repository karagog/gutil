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

#include "Core/DataObjects/private/list_p.h"
#include "Core/DataObjects/interfaces.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** A list of items, implemented as a memory array.

    The memory for the list is separated into pages, where each successive page is twice as large
    as the previous one.  This has an advantage over Vector's memory management, because
    there are no large memory reallocations when you run out.  In this case we just allocate a new
    page that effectively doubles the capacity of the list.

    It implements the Deque, Queue and Stack interfaces.

    \sa Vector, Dlist, SList
*/
template<class T>class List :
        protected list_p,
        public Stack<T>,
        public Deque<T>,
        public RandomAccessContainer<T>
{
public:

    /** How many items are in the list. */
    inline GUINT32 Size() const{ return count(); }

    /** How many items the list is able to hold without having to resize. */
    inline GUINT32 Capacity() const{ return capacity(); }

    /** Increases the list's capacity.  You should do this if you know how large you want
        the list to grow before-hand.
    */
    inline void Reserve(GUINT32 new_capacity){ reserve(new_capacity); }

    /** Inserts an item at the given index.
        Called any time an item is added to the list.
    */
    virtual void Insert(const T &i, GUINT32 indx){ insert(reinterpret_cast<void const *>(&i), indx); }

    /** Removes the item at the given index.
        Called any time an item is removed from the list.
        \note O(N) on average, unless you always remove from the end of the list.
    */
    virtual void Remove(GUINT32 indx){ remove(indx); }

    /** Appends an item to the list.
        \note O(1)
    */
    inline void Append(const T &i){ Insert(i, count()); }

    /** Appends another list on the end of this one.
        \returns A reference to this list.
        \note O(M), where M:=length(l)
    */
    inline List<T> &Append(const List<T> &l){
        Reserve(Size() + l.Size());
        for(int i(0); i < l.Size(); ++i) Append(l[i]);
        return *this;
    }

    /** Prepends an item to the list.
        \note O(N)
    */
    inline void Prepend(const T &i){ Insert(i, 0); }

    /** Prepends another list before this one.
        \returns A reference to this list.
        \note O(M*N), where M:=length(l) and N:=length(this)
    */
    inline List<T> &Prepend(const List<T> &l){
        Reserve(Size() + l.Size());
        for(int i(0); i < l.Size(); ++i) Insert(l[i], i);
        return *this;
    }


    /** A class to encapsulate the type-dependent function. */
    class TypeWrapper :
            public list_p::type_wrapper
    {
    public:
        virtual T *Copy(const T &o){ return new T(o); }
        virtual void Delete(T *o){ delete o; }

    private:
        void *CopyVoid(const void *const v) const{ return Copy(*reinterpret_cast<const T *const>(v)); }
        void DeleteVoid(void *v) const{ Delete(reinterpret_cast<T *>(v)); }
    };

    /** Constructs an empty list. */
    inline List()
        :list_p(new TypeWrapper)
    {}

    /** Constructs an empty list with the given storage capacity. */
    inline List(GUINT32 reserve_capacity)
        :list_p(new TypeWrapper){
        Reserve(reserve_capacity);
    }



    class iterator
    {
    public:
    };

    class const_iterator
    {
    public:
    };

    inline iterator begin(){}
    inline const_iterator begin() const{}
    inline iterator end(){}
    inline const_iterator end() const{}
    inline iterator rbegin(){}
    inline const_iterator rbegin() const{}
    inline iterator rend(){}
    inline const_iterator rend() const{}

};


GUTIL_END_CORE_NAMESPACE;

#endif // LIST_H
