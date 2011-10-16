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

#ifndef GUTIL_LIST_H
#define GUTIL_LIST_H

#include "Core/DataObjects/vector.h"
#include "Core/DataObjects/interfaces.h"
#include "gassert.h"
#include "gutil.h"
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
    friend class iterator;
    friend class const_iterator;
public:

    /** Constructs an empty list. */
    inline List()
        :d(0),
          m_size(0),
          m_pageCount(0)
    {}

    /** Constructs an empty list with the given storage capacity. */
    inline List(GUINT32 reserve_capacity)
        :m_size(0),
          m_pageCount(0)
    {
        Reserve(reserve_capacity);
    }

    inline List(const List &o)
        :m_size(0),
          m_pageCount(0)
    {
        Reserve(o.Capacity());

    }

    inline ~List(){ Clear(); }


    /** How many items are in the list. */
    inline GUINT32 Size() const{ return m_size; }

    /** How many items the list is able to hold without having to resize. */
    inline GUINT32 Capacity() const{ return capacity(m_pageCount); }

    /** Increases the list's capacity.  You should do this if you know how large you want
        the list to grow before-hand.
    */
    inline void Reserve(GUINT32 new_capacity)
    {
        GUINT32 cur;
        int new_pages(0);
        while((cur = Capacity()) < new_capacity)
        {
            ++m_pageCount;
            ++new_pages;
        }

        if(new_pages > 0)
        {
            // Allocate the new pages' memory
            for(int m(0); m < new_pages; ++m)
                d.Insert(new T [1 << (m_pageCount - new_pages + m + 1)], d.end());
        }
    }

    /** Inserts an item at the given index.
        Called any time an item is added to the list.
    */
    void Insert(const T &obj, GUINT32 indx)
    {
        GASSERT(0 <= indx && indx <= m_size);

        if((indx + 1) > capacity(m_pageCount))
            Reserve(indx + 1);

        if(indx < m_size)
        {
            // Shift subsequent items in the list by swapping
            GUINT32 i(m_size);
            T *prev( at(i) );
            T *cur( at(--i) );
            T *mem(0);

            // Call the constructor for the first item, because its memory is not initialized
            new(prev) T(*cur);
            prev = cur;

            while(i > indx)
            {
                cur = at(--i);
                if(IsMovableType<T>::Value)
                    gSwap(prev, cur, sizeof(T));
                else
                {
                    if(mem)
                        *mem = *prev;
                    else
                        mem = new T(*prev);
                    *prev = *cur;
                    *cur = *mem;
                }
                prev = cur;
            }

            if(mem) delete mem;

            // We insert the new item with the assignment operator in this branch of execution
            *cur = obj;
        }
        else
        {
            // Initialize the memory at the end of the array
            new(at(indx)) T(obj);
        }
        ++m_size;
    }

    /** Removes the item at the given index.
        Called any time an item is removed from the list.
        \note O(N) on average, unless you always remove from the end of the list.
    */
    void RemoveAt(GUINT32 indx)
    {
        GASSERT(indx <= m_size);

        // Call the item's destructor
        at(indx)->~T();

        if(indx < (m_size - 1))
        {
            // Shift all the subsequent items by swapping
            GUINT32 i(indx);
            T *prev( at(i) );
            T *cur( at(++i) );
            T *mem(0);

            // On the first item we call the constructor, because we just destructed it above
            new(prev) T(*cur);
            prev = cur;

            while(i < (m_size - 1))
            {
                cur = at(++i);
                if(IsMovableType<T>::Value)
                    gSwap(prev, cur, sizeof(T));
                else
                {
                    if(mem)
                        *mem = *prev;
                    else
                        mem = new T(*prev);
                    *prev = *cur;
                    *cur = *mem;
                }
                prev = cur;
            }
            if(mem) delete mem;

            // We call the destructor on the memory at the end
            cur->~T();
        }
        --m_size;
    }

    /** Removes the first instance of the item.  The type T must have a comparison operator.
        \note O(n)
    */
    inline void RemoveOne(const T &o)
    {
        for(GUINT32 i(0); i < m_size; ++i)
        {
            if(o == (*this)[i])
            {
                RemoveAt(i);
                break;
            }
        }
    }

    /** Removes the last instance of the item.  The type T must have a comparison operator.
        \note O(n)
    */
    inline void RemoveLast(const T &o)
    {
        for(GUINT32 i(0); i < m_size; ++i)
        {
            const GUINT32 cur( m_size - i - 1 );
            if(o == (*this)[cur])
            {
                RemoveAt(cur);
                break;
            }
        }
    }

    /** Removes all instances of the item.  The type T must have a comparison operator.
        \note O(n)
    */
    inline void RemoveAll(const T &o)
    {
        for(GUINT32 i(0); i < m_size; ++i)
        {
            const GUINT32 cur( m_size - 1 - i );
            if(o == (*this)[cur])
                RemoveAt(cur);
        }
    }

    /** Removes all items and clears all memory. */
    void Clear()
    {
        // Call the destructor on all our items
        for(GUINT32 i(0); i < m_size; ++i)
            at(i)->~T();

        // Free our page memory
        for(int p(0); p < m_pageCount; ++p)
            delete[] d[p];

        d.Clear();
        m_size = 0;
        m_pageCount = 0;
    }

    /** Appends an item to the list.
        \note O(1)
    */
    inline void Append(const T &i){ Insert(i, m_size); }

    /** Appends another list on the end of this one.
        \returns A reference to this list.
        \note O(M), where M:=length(l)
    */
    inline List<T> &Append(const List<T> &l){
        Reserve(Size() + l.Size());
        for(int i(0); i < l.Size(); ++i) Append(l[i]);
        return *this;
    }

    /** Removes the first item in the list. */
    inline void RemoveLast(){ RemoveAt(m_size - 1); }

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

    /** Removes the last item in the list. */
    inline void RemoveFirst(){ RemoveAt(0); }

    /** Returns the item at the given index.  Does not do bounds checking. */
    inline const T &operator [](GUINT32 indx) const{ return *at(indx); }
    /** Returns the item at the given index.  Does not do bounds checking. */
    inline T &operator [](GUINT32 indx){ return *at(indx); }



    class iterator
    {
        friend class List;
    public:

        inline iterator() :d(0), current(0){}

        inline iterator &operator ++(){ ++current; return *this;}
        inline iterator operator ++(int){ iterator ret(*this); ++current; return ret; }
        inline iterator &operator +=(int n){ current += n; return *this; }
        inline iterator operator +(int n){ iterator ret(*this); ret.current += n; return ret; }

        inline iterator &operator --(){ --current; return *this;}
        inline iterator operator --(int){ iterator ret(*this); --current; return ret; }
        inline iterator &operator -=(int n){ current -= n; return *this; }
        inline iterator operator -(int n){ iterator ret(*this); ret.current -= n; return ret; }

        T *operator->(){ return d->at(current); }
        const T *operator->() const{ return d->at(current); }
        T &operator*(){ return *d->at(current); }
        const T &operator*() const{ return *d->at(current); }

    protected:

        inline iterator(List<T> *sl, GUINT32 cur)
            :d(sl), current(cur){}

        List<T> *d;
        GUINT32 current;

    };

    class const_iterator
    {
        friend class List;
    public:

        inline const_iterator() :d(0), current(0){}
        inline const_iterator(const iterator &o) :d(o.d), current(o.current){}

        inline const_iterator &operator ++(){ ++current; return *this;}
        inline const_iterator operator ++(int){ const_iterator ret(*this); ++current; return ret; }
        inline const_iterator &operator +=(int n){ current += n; return *this; }
        inline const_iterator operator +(int n){ const_iterator ret(*this); ret.current += n; return ret; }

        inline const_iterator &operator --(){ --current; return *this;}
        inline const_iterator operator --(int){ const_iterator ret(*this); --current; return ret; }
        inline const_iterator &operator -=(int n){ current -= n; return *this; }
        inline const_iterator operator -(int n){ const_iterator ret(*this); ret.current -= n; return ret; }

        T *operator->(){ return d->at(current); }
        const T *operator->() const{ return d->at(current); }
        T &operator*(){ return *d->at(current); }
        const T &operator*() const{ return *d->at(current); }


    protected:

        inline const_iterator(List<T> &sl, GUINT32 cur)
            :d(sl), current(cur){}

        List<T> *d;
        GUINT32 current;

    };

    inline iterator begin(){ return m_size > 0 ? iterator(this, 0) : iterator(); }
    inline const_iterator begin() const{ return m_size > 0 ? const_iterator(this, 0) : const_iterator(); }
    inline iterator end(){ return m_size > 0 ? iterator(this, m_size) : iterator(); }
    inline const_iterator end() const{ return m_size > 0 ? const_iterator(this, m_size) : const_iterator(); }

    inline iterator rbegin(){ return m_size > 0 ? iterator(this, m_size - 1) : iterator(); }
    inline const_iterator rbegin() const{ return m_size > 0 ? const_iterator(this, m_size - 1) : const_iterator();}
    inline iterator rend(){ return m_size > 0 ? iterator(this, -1) : iterator(); }
    inline const_iterator rend() const{ return m_size > 0 ? const_iterator(this, -1) : const_iterator(); }


protected:

    static inline GUINT32 capacity(GUINT32 number_of_pages){ return (1 << number_of_pages) - 1; }

    inline T *at(GUINT32 indx) const{
        const int pindx( FSB64(++indx) );
        return d[pindx] + TRUNCATE_LEFT_32(indx, 32 - pindx);
    }


private:

    Vector<T*> d;
    GUINT32 m_size;
    int m_pageCount;

};



template<class T>class ListStack : public Stack<T>
{
    GUTIL_DISABLE_COPY(ListStack<T>)
public:

    inline ListStack(List<T> *lst) :m_list(lst), m_delete(false){}
    inline ListStack() :m_list(new List<T>), m_delete(true){}
    inline ~ListStack(){ if(m_delete) delete m_list; }

    void Push(const T &o){ m_list->Append(o); }
    void Pop(){ m_list->RemoveLast(); }
    const T &Top() const{ return (*m_list)[m_list->Size() - 1]; }
    T &Top(){ return (*m_list)[m_list->Size() - 1]; }
    GUINT32 CountStackItems() const{ return m_list->Size(); }
    void FlushStack(){ m_list->Clear(); }


private:

    List<T> *m_list;
    bool m_delete;

};


template<class T>class ListQueue : public Queue<T>
{
    GUTIL_DISABLE_COPY(ListQueue<T>)
public:

    inline ListQueue(List<T> *lst) :m_list(lst), m_delete(false){}
    inline ListQueue() :m_list(new List<T>), m_delete(true){}
    inline ~ListQueue(){ if(m_delete) delete m_list; }

    void Enqueue(const T &o){ m_list->Append(o); }
    void Dequeue(){ m_list->RemoveFirst(); }
    T &Front(){ return (*m_list)[0]; }
    const T &Front() const{ return (*m_list)[0]; }
    void FlushQueue(){ m_list->Clear(); }
    GUINT32 CountQueueItems() const{ return m_list->Size(); }


private:

    List<T> *m_list;
    bool m_delete;

};


template<class T>class ListDeque : public Deque<T>
{
    GUTIL_DISABLE_COPY(ListDeque<T>)
public:

    inline ListDeque(List<T> *lst) :m_list(lst), m_delete(false){}
    inline ListDeque() :m_list(new List<T>), m_delete(true){}
    inline ~ListDeque(){ if(m_delete) delete m_list; }

    void PushBack(const T &o){ m_list->Append(o); }
    void PushFront(const T &o){ m_list->Prepend(o); }
    void PopBack(){ m_list->RemoveLast(); }
    void PopFront(){ m_list->RemoveFirst(); }
    const T &Front() const{ return (*m_list)[0]; }
    T &Front(){ return (*m_list)[0]; }
    const T &Back() const{ return (*m_list)[m_list->Size() - 1]; }
    T &Back(){ return (*m_list)[m_list->Size() - 1]; }
    GUINT32 CountDequeItems() const{ return m_list->Size(); }
    void FlushDeque(){ m_list->Clear(); }


private:

    List<T> *m_list;
    bool m_delete;

};


template<class T>class ListRandomAccessContainer : public RandomAccessContainer<T>
{
    GUTIL_DISABLE_COPY(ListRandomAccessContainer<T>)
public:

    inline ListRandomAccessContainer(List<T> *lst) :m_list(lst), m_delete(false){}
    inline ListRandomAccessContainer() :m_list(new List<T>), m_delete(true){}
    inline ~ListRandomAccessContainer(){ if(m_delete) delete m_list; }

    const T &At(GUINT32 i) const{ return (*m_list)[i]; }
    T &At(GUINT32 i){ return (*m_list)[i]; }
    void InsertAt(const T &i, GUINT32 indx){ m_list->Insert(i, indx); }
    void RemoveAt(GUINT32 indx){ m_list->RemoveAt(indx); }
    GUINT32 CountContainerItems() const{ return m_list->Size(); }
    void FlushContainer(){ m_list->Clear(); }


private:

    List<T> *m_list;
    bool m_delete;

};


GUTIL_END_CORE_NAMESPACE;


namespace GUtil
{

template<class T>struct IsMovableType< Core::DataObjects::List<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::ListStack<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::ListQueue<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::ListDeque<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::ListRandomAccessContainer<T> >{ enum{ Value = 1 }; };

}


#endif // GUTIL_LIST_H
