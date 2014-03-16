/*Copyright 2010-2013 George Karagoulis

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

#include "gutil_vector.h"
#include "gutil_interfaces.h"
#include "gutil_macros.h"
NAMESPACE_GUTIL1(DataObjects);


/** A list of items, implemented as a memory array.

    The memory for the list is separated into pages, where each successive page is twice as large
    as the previous one.  This has an advantage over Vector's memory management, because
    there are no large memory reallocations when you run out.  In this case we just allocate a new
    page that effectively doubles the capacity of the list.

    It implements the Deque, Queue and Stack interfaces.

    \sa Vector, Dlist, SList
*/
template<class T>class ListImp
{
    friend class iterator;
    friend class const_iterator;
public:

    /** Constructs an empty list. */
    ListImp()
        :m_pageCount(0),
          m_size(0)
    {}

    /** Constructs an empty list with the given storage capacity. */
    ListImp(GINT32 reserve_capacity)
        :m_pageCount(0),
          m_size(0)
    {
        Reserve(reserve_capacity);
    }

    /** Constructs a list with the given number of item copies */
    explicit ListImp(const T &item, GINT32 count = 1)
        :m_pageCount(0),
          m_size(0)
    {
        Reserve(count);
        while(count-- != 0) Append(item);
    }

    /** Conducts a deep copy of the list */
    ListImp(const ListImp &o)
        :m_pageCount(0),
          m_size(0)
    {
        Reserve(o.Capacity());

        for(const_iterator iter(o.begin()); iter != o.end(); ++iter)
            Append(*iter);
    }

    /** Conducts a deep copy of the list */
    ListImp<T> &operator = (const ListImp &o){
        Empty();
        new(this) ListImp<T>(o);
        return *this;
    }

    ~ListImp(){ Clear(); }


    /** How many items are in the list. */
    GINT32 Length() const{ return m_size; }

    /** How many items the list is able to hold without having to resize. */
    GINT32 Capacity() const{ return capacity(m_pageCount); }

    /** Increases the list's capacity.  You should do this if you know how large you want
        the list to grow before-hand.
    */
    void Reserve(GINT32 new_capacity)
    {
        GINT32 cur;
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
            {
                void *new_data( malloc(sizeof(T) * (1 << (m_pageCount - new_pages + m + 1))) );
                if(!new_data)
                    THROW_NEW_GUTIL_EXCEPTION(BadAllocationException);
                d.Insert(reinterpret_cast<T *>(new_data), d.end());
            }
        }
    }

    /** Inserts an item at the given index.
        Called any time an item is added to the list.
    */
    T &Insert(const T &obj, GINT32 indx)
    {
        GASSERT(indx <= m_size);

        if((indx + 1) > capacity(m_pageCount))
            Reserve(indx + 1);

        if(indx < m_size)
        {
            // Shift subsequent items in the list by swapping
            GINT32 i(m_size);
            iterator prev(end());
            iterator cur(rbegin());
            GBYTE mem_data[sizeof(T)];
            T *mem(NULL);

            // Call the constructor for the first item, because its memory is not initialized
            new(&(*prev)) T(*cur);
            --prev, --i;

            while(i > indx)
            {
                --cur;
                if(mem)
                    *mem = *prev;
                else
                {
                    mem = reinterpret_cast<T *>(mem_data);
                    new(mem) T(*prev);
                }

                *prev = *cur;
                *cur = *mem;
                --prev, --i;
            }

            // We insert the new item with the assignment operator in this branch of execution
            *cur = obj;
        }
        else
        {
            // Initialize the memory at the end of the array
            indx = m_size;
            new(at(indx)) T(obj);
        }

        ++m_size;

        return this->operator [](indx);
    }

    /** Removes the item at the given index.
        Called any time an item is removed from the list.
        \note O(N) on average, unless you always remove from the end of the list.
    */
    void RemoveAt(GINT32 indx)
    {
        GASSERT(indx <= m_size);

        // Call the item's destructor
        iterator cur(&d, indx);
        cur->~T();

        if(indx < (m_size - 1))
        {
            // Shift all the subsequent items by swapping
            GINT32 i(indx);
            iterator prev(cur);
            char mem_data[sizeof(T)];
            T *mem(NULL);

            // On the first item we call the constructor, because we just destructed it above
            new(&(*prev)) T(*(++cur));
            ++prev, ++i;

            while(i < (m_size - 1))
            {
                ++cur;
                if(mem)
                    *mem = *prev;
                else
                {
                    mem = reinterpret_cast<T *>(mem_data);
                    new(mem) T(*prev);
                }

                *prev = *cur;
                *cur = *mem;
                ++prev, ++i;
            }

            // We call the destructor on the memory at the end
            cur->~T();
        }
        --m_size;
    }

    /** Removes the first instance of the item.  The type T must have a comparison operator.
        \note O(n)
    */
    void RemoveOne(const T &o)
    {
        for(GINT32 i(0); i < m_size; ++i)
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
    void RemoveLast(const T &o)
    {
        for(GINT32 i(0); i < m_size; ++i)
        {
            const GINT32 cur( m_size - i - 1 );
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
    void RemoveAll(const T &o)
    {
        for(GINT32 i(0); i < m_size; ++i)
        {
            const GINT32 cur( m_size - 1 - i );
            if(o == (*this)[cur])
                RemoveAt(cur);
        }
    }

    /** Removes all items and clears all memory. */
    void Clear()
    {
        // Call the destructor on all the items
        for(iterator iter(begin()); iter != end(); ++iter)
            iter->~T();

        // Free our page memory
        for(int p(0); p < m_pageCount; ++p)
            free( d[p] );

        d.Clear();
        m_size = 0;
        m_pageCount = 0;
    }

    /** Removes all the items, but retains the same capacity. */
    void Empty()
    {
        // Call the destructor on all the items
        for(iterator iter(begin()); iter != end(); ++iter)
            iter->~T();
        m_size = 0;
    }

    /** Appends an item to the list.
        \note O(1)
    */
    T &Append(const T &i){ return Insert(i, m_size); }

    /** Appends another list on the end of this one.
        \returns A reference to this list.
        \note O(M), where M:=length(l)
    */
    ListImp<T> &Append(const ListImp<T> &l){
        Reserve(Length() + l.Length());
        for(GINT32 i(0); i < l.Length(); ++i) Append(l[i]);
        return *this;
    }

    /** Removes the first item in the list. */
    void RemoveLast(){ RemoveAt(m_size - 1); }

    /** Prepends an item to the list.
        \note O(N)
    */
    T &Prepend(const T &i){ return Insert(i, 0); }

    /** Prepends another list before this one.
        \returns A reference to this list.
        \note O(M*N), where M:=length(l) and N:=length(this)
    */
    ListImp<T> &Prepend(const ListImp<T> &l){
        Reserve(Length() + l.Length());
        for(GINT32 i(0); i < l.Length(); ++i) Insert(l[i], i);
        return *this;
    }

    /** Removes the last item in the list. */
    void RemoveFirst(){ RemoveAt(0); }

    /** Returns the item at the given index.  Does not do bounds checking. */
    const T &operator [](GINT32 indx) const{ return *at(indx); }
    /** Returns the item at the given index.  Does not do bounds checking. */
    T &operator [](GINT32 indx){ return *at(indx); }

    /** Returns the item at the given index.  Throws an exception if index out of range. */
    const T &At(GINT32 indx) const{
        if(indx >= Length()) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return *at(indx);
    }
    /** Returns the item at the given index.  Throws an exception if index out of range. */
    T &At(GINT32 indx){
        if(indx >= Length()) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return *at(indx);
    }


    /** Pushes an item on a logical stack, with appealing syntax. */
    ListImp<T> &operator << (const T &item){ Append(item); return *this; }

    /** Pops the top item from a logical stack and copies it into the given variable */
    ListImp<T> &operator >> (T &cpy){ cpy = *rbegin(); RemoveLast(); return *this; }


    /** Sorts the list using the given sorting algorithm. */
    void Sort(bool ascending = true, GUtil::SortTypeEnum e = MergeSort, const Interfaces::IComparer<T> &comparer = GUtil::DefaultComparer<T>()){
        switch(e)
        {
        case GUtil::MergeSort:
        {
            Vector<T> buffer;
            buffer.ReserveExactly(Length());
            _merge_sort(begin(), end(), ascending, buffer, comparer);
        }
            break;
        default:
            THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
        }
    }



    class iterator
    {
        friend class ListImp;
    public:

        /** Returns the index we're currently on. */
        GINT32 Index() const{
            return (1 << m_pageIndex) + (current - m_pageBegin) - 1;
	}

        iterator &operator ++(){ _advance(); return *this;}
        iterator operator ++(int){ iterator ret(*this); _advance(); return ret; }
        iterator &operator += (GINT32 n){ while(n-- != 0) _advance(); return *this; }
        iterator operator + (GINT32 n) const{ iterator ret(*this); while(n-- != 0) ret._advance(); return ret; }

        iterator &operator --(){ _retreat(); return *this;}
        iterator operator --(int){ iterator ret(*this); _retreat(); return ret; }
        iterator &operator -= (GINT32 n){ while(n-- != 0) _retreat(); return *this; }
        iterator operator - (GINT32 n) const{ iterator ret(*this); while(n-- != 0) ret._retreat(); return ret; }

        /** Returns the distance between iterators. */
        GINT32 operator - (const iterator &o) const{
            GINT32 ret(0);
            T *tmp_cur(o.current);
            T *tmp_end(o.m_pageEnd);
            for(GINT32 cur_page(o.m_pageIndex); cur_page < m_pageIndex; ++cur_page)
            {
                ret += tmp_end - tmp_cur;
                tmp_cur = m_pages->operator [](cur_page + 1);
                tmp_end = tmp_cur + (1 << (cur_page + 1));
            }
            return ret + (current - tmp_cur);
        }

        bool operator == (const iterator &o) const{
            return m_pages->ConstData() == o.m_pages->ConstData() && current == o.current;
        }
        bool operator != (const iterator &o) const{
            return m_pages->ConstData() != o.m_pages->ConstData() || current != o.current;
        }

        T *operator->() const{ return current; }
        T &operator*() const{ return *current; }


    protected:

        iterator(const Vector<T *> *pages, GINT32 indx)
            :m_pages(pages),
              m_pageIndex(FSB32(indx + 1)),
              m_pageBegin(m_pageIndex >= 0 && m_pages->IsNull() ? 0 : m_pages->operator [](m_pageIndex)),
              m_pageEnd(m_pageIndex >= 0 && m_pages->IsNull() ? 0 : m_pageBegin + (1 << m_pageIndex)),
              current(m_pageIndex >= 0 && m_pages->IsNull() ? 0 : m_pageBegin + TRUNCATE_LEFT_32(indx + 1, 32 - m_pageIndex))
        {}


        Vector<T *> const *m_pages;
        GINT32 m_pageIndex;
        T *m_pageBegin;
        T *m_pageEnd;
        T *current;

        void _advance(){
            if(!current || ++current == m_pageEnd)
            {
                if((GINT32)++m_pageIndex >= m_pages->Length())
                {
                    m_pageBegin = 0;
                    m_pageEnd = 0;
                    current = 0;
                }
                else
                {
                    m_pageBegin = m_pages->operator [](m_pageIndex);
                    m_pageEnd = m_pageBegin + (1 << m_pageIndex);
                    current = m_pageBegin;
                }
            }

        }

        void _retreat(){
            if(!current || current-- == m_pageBegin)
            {
                if(--m_pageIndex < 0)
                {
                    m_pageBegin = 0;
                    m_pageEnd = 0;
                    current = 0;
                }
                else
                {
                    m_pageBegin = m_pages->operator [](m_pageIndex);
                    m_pageEnd = m_pageBegin + (1 << m_pageIndex);
                    current = m_pageEnd - 1;
                }
            }
        }

    };

    class const_iterator
    {
        friend class ListImp;
    public:

        const_iterator(const iterator &o)
            :m_pages(o.m_pages),
              m_pageIndex(o.m_pageIndex),
              m_pageBegin(o.m_pageBegin),
              m_pageEnd(o.m_pageEnd),
              current(o.current)
        {}

        /** Returns the index we're currently on. */
        GINT32 Index() const{
            return (1 << m_pageIndex) + (current - m_pageBegin) - 1;
        }

        const_iterator &operator ++(){ _advance(); return *this;}
        const_iterator operator ++(int){ const_iterator ret(*this); _advance(); return ret; }
        const_iterator &operator += (GINT32 n){ while(n-- != 0) _advance(); return *this; }
        const_iterator operator + (GINT32 n) const{ const_iterator ret(*this); while(n-- != 0) ret._advance(); return ret; }

        const_iterator &operator --(){ _retreat(); return *this;}
        const_iterator operator --(int){ const_iterator ret(*this); _retreat(); return ret; }
        const_iterator &operator -=(GINT32 n){ while(n-- != 0) _retreat(); return *this; }
        const_iterator operator -(GINT32 n) const{ const_iterator ret(*this); while(n-- != 0) ret._retreat(); return ret; }

        /** Returns the distance between iterators. */
        GINT32 operator - (const const_iterator &o) const{
            GINT32 ret(0);
            T *tmp_cur(o.current);
            T *tmp_end(o.m_pageEnd);
            for(GINT32 cur_page(o.m_pageIndex); cur_page < m_pageIndex; ++cur_page)
            {
                ret += tmp_end - tmp_cur;
                tmp_cur = m_pages->operator [](cur_page + 1);
                tmp_end = tmp_cur + (1 << (cur_page + 1));
            }
            return ret + (current - tmp_cur);
        }

        bool operator == (const const_iterator &o) const{
            return m_pages->ConstData() == o.m_pages->ConstData() && current == o.current;
        }
        bool operator != (const const_iterator &o) const{
            return m_pages->ConstData() != o.m_pages->ConstData() || current != o.current;
        }

        const T *operator->() const{ return current; }
        const T &operator*() const{ return *current; }


    protected:

        const_iterator(const Vector<T *> *pages, GINT32 indx)
            :m_pages(pages),
              m_pageIndex(FSB32(indx + 1)),
              m_pageBegin(m_pageIndex >= 0 && m_pages->IsNull() ? 0 : m_pages->operator [](m_pageIndex)),
              m_pageEnd(m_pageIndex >= 0 && m_pages->IsNull() ? 0 : m_pageBegin + (1 << m_pageIndex)),
              current(m_pageIndex >= 0 && m_pages->IsNull() ? 0 : m_pageBegin + TRUNCATE_LEFT_32(indx + 1, 32 - m_pageIndex))
        {}


        Vector<T *> const *m_pages;
        GINT32 m_pageIndex;
        T *m_pageBegin;
        T *m_pageEnd;
        T *current;

        void _advance(){
            if(!current || ++current == m_pageEnd)
            {
                if((GINT32)++m_pageIndex >= m_pages->Length())
                {
                    m_pageBegin = 0;
                    m_pageEnd = 0;
                    current = 0;
                }
                else
                {
                    m_pageBegin = m_pages->operator [](m_pageIndex);
                    m_pageEnd = m_pageBegin + (1 << m_pageIndex);
                    current = m_pageBegin;
                }
            }

        }

        void _retreat(){
            if(!current || current-- == m_pageBegin)
            {
                if(--m_pageIndex < 0)
                {
                    m_pageBegin = 0;
                    m_pageEnd = 0;
                    current = 0;
                }
                else
                {
                    m_pageBegin = m_pages->operator [](m_pageIndex);
                    m_pageEnd = m_pageBegin + (1 << m_pageIndex);
                    current = m_pageEnd - 1;
                }
            }
        }

    };

    iterator begin(){ return iterator(&d, 0); }
    const_iterator begin() const{ return const_iterator(&d, 0); }
    iterator end(){ return 0 == m_size ? iterator(&d, 0) : ++iterator(&d, m_size - 1); }
    const_iterator end() const{ return 0 == m_size ? const_iterator(&d, 0) : ++const_iterator(&d, m_size - 1); }

    iterator rbegin(){ return iterator(&d, m_size - 1); }
    const_iterator rbegin() const{ return const_iterator(&d, m_size - 1);}
    iterator rend(){ return iterator(&d, -1); }
    const_iterator rend() const{ return const_iterator(&d, -1); }

    /** Returns the item at the front of the list */
    T &Front(){ return *d.Front(); }
    /** Returns the item at the front of the list */
    T const &Front() const{ return *d.Front(); }

    /** Returns the item at the back of the list */
    T &Back(){ return *at(Length() - 1); }
    /** Returns the item at the back of the list */
    T const &Back() const{ return *at(Length() - 1); }


protected:

    static GINT32 capacity(GINT32 number_of_pages){ return (1 << number_of_pages) - 1; }

    T *at(GINT32 indx) const{
        const int pindx( FSB32(++indx) );
        return d[pindx] + TRUNCATE_LEFT_32(indx, 32 - pindx);
    }


private:

    Vector<T*> d;
    int m_pageCount;
    GINT32 m_size;

    void _merge_sort(const iterator &b, const iterator &e, bool ascending, Vector<T> &buffer, const Interfaces::IComparer<T> &cmp){
        GINT32 diff( e - b );
        if(diff == INT_MAX);
        else if(diff == 2)
        {
            T *last( &(*(e - 1)) );
            if((ascending && 0 < cmp(*b, *last)) ||
               (!ascending && 0 > cmp(*b, *last)))
            {
                T cpy(*b);
                *b = *last;
                *last = cpy;
            }
        }
        else if(diff > 2)
        {
            iterator m(b + (diff / 2));

            // Sort the left and right halves of the list
            _merge_sort(b, m, ascending, buffer, cmp);
            _merge_sort(m, e, ascending, buffer, cmp);

            // Join the two halves, which are already sorted
            iterator i1(b), i2(m);
            while((i1 != m) && (i2 != e))
            {
                if((ascending && 0 < cmp(*i1, *i2)) ||
                   (!ascending && 0 > cmp(*i1, *i2)))
                {
                    buffer.PushBack(*i2);
                    ++i2;
                }
                else
                {
                    buffer.PushBack(*i1);
                    ++i1;
                }
            }
            while(i1 != m)
            {
                buffer.PushBack(*i1);
                ++i1;
            }
            while(i2 != e)
            {
                buffer.PushBack(*i2);
                ++i2;
            }

            // Copy the now-sorted buffer back to the original
            i1 = b;
            typename Vector<T>::iterator i3( buffer.begin() );
            while(i1 != e)
            {
                *i1 = *i3;
                ++i1, ++i3;
            }

            // Clear the items in the buffer
            buffer.Empty();
        }
    }

};



/** This is used to define all the same constructors (and virtual destructor) as the base class so you don't have to
 *  repeat so much code.
 *
 *  \param class_name The name of top-level class for which to define constructors.
 *  \param base_class The base class to whose constructors you will forward parameters.
*/
#define GUTIL_LIST_CONSTRUCTORS(class_name, base_class) \
    public: \
    class_name(){} \
    class_name(GINT32 reserve_capacity) :base_class(reserve_capacity){} \
    explicit class_name(const T &item, GINT32 count = 1) :base_class(item, count){} \
    class_name(const base_class &o) :base_class(o){} \
    virtual ~class_name(){}




/** Defines the basic list type without any interfaces.
 *
 *  Use this as the most memory efficient version.
 *
 *  \tparam T The type contained by the vector.
 *  \tparam IFace The interface class, or void if no interface.
*/
template<class T, typename IFace = void> class List : public ListImp<T>
{ GUTIL_LIST_CONSTRUCTORS(List, ListImp<T>) };




/** Defines a template specialization of the List class, with a random access container interface. */
template<class T> class List<T, IRandomAccessContainer<T> > :
        public List<T>,
        public IRandomAccessContainer<T>
{
    GUTIL_VECTOR_CONSTRUCTORS(List, List<T>)

    virtual T &At(GINT32 i){ return ListImp<T>::At(i); }
    virtual T const &At(GINT32 i) const{ return ListImp<T>::At(i); }

    virtual T &Insert(const T &item, GINT32 i){ return ListImp<T>::Insert(item, i); }
    virtual void Remove(GINT32 i){ ListImp<T>::RemoveAt(i); }

    virtual GINT32 Size() const{ return ListImp<T>::Length(); }
    virtual void Clear(){ ListImp<T>::Empty(); }

};



/** Defines a template specialization of the List class, with a stack interface. */
template<class T> class List<T, IStack<T> > :
        public List<T>,
        public IStack<T>
{
    GUTIL_VECTOR_CONSTRUCTORS(List, List<T>)

    virtual T &Push(const T &item){ return ListImp<T>::Append(item); }
    virtual void Pop(){ ListImp<T>::RemoveLast(); }
    virtual T &Top(){ return ListImp<T>::Back(); }
    virtual T const &Top() const{ return ListImp<T>::Back(); }

    virtual GINT32 Size() const{ return ListImp<T>::Length(); }
    virtual void Clear(){ ListImp<T>::Empty(); }

};


/** Defines a template specialization of the List class, with a queue interface. */
template<class T> class List<T, IQueue<T> > :
        public List<T>,
        public IQueue<T>
{
    GUTIL_VECTOR_CONSTRUCTORS(List, List<T>)

    virtual T &Enqueue(const T &item){ return ListImp<T>::Append(item); }
    virtual void Dequeue(){ ListImp<T>::RemoveFirst(); }
    virtual T &Front(){ return ListImp<T>::Front(); }
    virtual T const &Front() const{ return ListImp<T>::Front(); }

    virtual GINT32 Size() const{ return ListImp<T>::Length(); }
    virtual void Clear(){ ListImp<T>::Empty(); }

};


/** Defines a template specialization of the List class, with a deque interface. */
template<class T> class List<T, IDeque<T> > :
        public List<T>,
        public IDeque<T>
{
    GUTIL_VECTOR_CONSTRUCTORS(List, List<T>)

    virtual T &EnqueueFront(const T &item){ return ListImp<T>::Prepend(item); }
    virtual T &EnqueueBack(const T &item){ return ListImp<T>::Append(item); }
    virtual void DequeueFront(){ ListImp<T>::RemoveFirst(); }
    virtual void DequeueBack(){ ListImp<T>::RemoveLast(); }
    virtual T &Front(){ return ListImp<T>::Front(); }
    virtual T const &Front() const{ return ListImp<T>::Front(); }
    virtual T &Back(){ return ListImp<T>::Back(); }
    virtual T const &Back() const{ return ListImp<T>::Back(); }

    virtual GINT32 Size() const{ return ListImp<T>::Length(); }
    virtual void Clear(){ ListImp<T>::Empty(); }

};


END_NAMESPACE_GUTIL1;


namespace GUtil
{

template<class T>struct IsMovableType< DataObjects::ListImp<T> >{ enum{ Value = 1 }; };
template<class T, class U>struct IsMovableType< DataObjects::List<T, U> >{ enum{ Value = 1 }; };

}


#endif // GUTIL_LIST_H
