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

#ifndef GUTIL_VECTOR_P
#define GUTIL_VECTOR_P

#include "gutil_macros.h"
#include "Core/globals.h"
#include "Core/exception.h"
#include "Core/DataObjects/interfaces.h"
#include <new>
#include <limits.h>
#include <cstdlib>
#include <cstring>
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Manages a simple vector of memory, which grows exponentially with powers of two.
    You call reserve() with the number of items you want to have, and it will reserve
    at least as many, only conducting memory reallocations when necessary.
*/
template<class T>class Vector
{
public:
    class iterator;
    class const_iterator;

    /** Constructs an empty vector. */
    inline Vector() :m_begin(NULL) {}

    /** Constructs an empty vector capable of holding the given number of items. */
    inline explicit Vector(GUINT32 capacity)
        :m_begin(NULL)
    {
        if(capacity > 0)
            Reserve(capacity);
    }


    /**  Constructs a vector of the given size, where all elements are copies of the provided object. */
    inline Vector(const T &o, GUINT32 size)
        :m_begin(NULL)
    {
        Reserve(size);
        _set_length(size);
        T *cur(m_begin);
        for(GUINT32 i(0); i < size; ++i, ++cur)
            new(cur) T(o);
    }


    /**  Constructs a vector from the array of data. */
    inline Vector(T const*arr, GUINT32 size)
        :m_begin(NULL)
    {
        if(size > 0)
        {
            Reserve(size);
            _set_length(size);
            T *cur(m_begin);
            for(GUINT32 i(0); i < size; ++i)
                new(cur++) T(*(arr++));
        }
    }


    /** Creates a vector with the elements from another vector, taken by iterating from
        iter_begin to iter_end.
    */
    inline Vector(const typename Vector<T>::const_iterator &iter_begin,
                        const typename Vector<T>::const_iterator &iter_end)
        :m_begin(NULL)
    {
        const GUINT32 sz( iter_end - iter_begin );
        if(sz > 0)
            Reserve(sz);
        T *ptr(m_begin);
        for(Vector<T>::const_iterator cur(iter_begin); cur != iter_end;)
            new(ptr++) T(*(cur++));
    }


    /** The copy constructor conducts a deep copy, invoking the copy constructors on each item. */
    Vector(const Vector<T> &o)
        :m_begin(NULL)
    {
        const GUINT32 sz( o.Length() );
        if(o.Capacity() > 0)
        {
            Reserve(o.Capacity());
            _set_length(sz);
        }

        // Call the copy constructor for each item to initialize the memory
        T *cur( m_begin );
        T *ocur( o.m_begin );
        for(GUINT32 i(0); i < Length(); ++i)
            new(cur++) T(*(ocur++));
    }
    /** Assignment operator invokes our copy constructor after clearing the container. */
    Vector &operator = (const Vector<T> &o){
        Clear();
        new(this) Vector<T>(o);
        return *this;
    }
    inline ~Vector(){ Clear(); }

    /** Insert the item at the position before the iterator.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    inline void Insert(const T &item, const iterator &iter){ Insert(item, iter.Index()); }

    /** Insert the item at the index position.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    void Insert(const T &item, GUINT32 indx)
    {
        const GUINT32 len( Length() );

        // Allocate more memory if we have to
        if(len == Capacity())
            Reserve(len + 1);

        T *dest( m_begin + indx );

        // Move the destination out of the way, if they're inserting anywhere but the end
        if(indx < len)
        {
            if(IsMovableType<T>::Value)
            {
                memmove(dest + 1, dest, (len - indx) * sizeof(T));
                new(dest) T(item);
            }
            else
            {
                T *cur(m_begin + len - 1);

                // Call the constructor on the memory location at the end
                new(m_begin + len) T(*cur);

                if(len > 0)
                {
                    for(GUINT32 i(len - 1); i > indx; --i, --cur)
                        *cur = *(cur - 1);
                }

                // Then assign the item to the proper location
                m_begin[indx] = item;
            }
        }
        else
        {
            // Call the copy constructor to initialize the memory at the end of the array
            new(dest) T(item);
        }

        _set_length( len + 1 );
    }

    /** Insert the vector at the index position.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    inline void Insert(const Vector<T> &vec, GUINT32 indx){
        Insert(vec.ConstData(), vec.Size(), indx);
    }

    /** Insert the array at the index position.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    void Insert(const T *vec, GUINT32 size, GUINT32 indx)
    {
        const GUINT32 len( Length() );

        // Allocate more memory if we have to
        if((len + size) > Capacity())
            Reserve(Length() + size);

        T *dest( m_begin + indx );

        // Move the destination out of the way, if they're inserting anywhere but the end
        if(indx < len)
        {
            if(IsMovableType<T>::Value)
            {
                memmove(dest + size, dest, (len - indx) * sizeof(T));
                for(GUINT32 i(0); i < size; ++i)
                    new(dest++) T(*(vec++));
            }
            else
            {
                // Call the constructors on the memory location at the end
                for(GUINT32 i(1); i <= len && i <= size; ++i)
                    new(m_begin + len + size - i) T(*(m_begin + len - i));

                T *cur(m_begin + len - 1);
                if(len > 0)
                {
                    for(GUINT32 i(len - 1); i > (indx + size); --i, --cur)
                        *cur = *(cur - size);
                }

                // Then assign the items to the proper location
                for(GUINT32 i(0); i < size; ++i)
                {
                    if(i < len)
                        m_begin[indx + i] = vec[i];
                    else
                        new(m_begin + indx + i) T(vec[i]);
                }
            }
        }
        else
        {
            // Call the copy constructors to initialize the memory at the end of the array
            for(GUINT32 i(0); i < size; ++i)
                new(dest++) T(*(vec++));
        }

        _set_length(len + size);
    }

    /** Remove the item pointed to by the iterator.

        \note Invalidates the iterator positioned at the last element, and all other iterators
        after the input would notice their current element has shifted
    */
    inline void Remove(const iterator &iter){ RemoveAt(iter.Index()); }

    /** Remove the item at the index location.

        \note Invalidates the iterator positioned at the last element, and all other iterators
        after the input would notice their current element has shifted
        \note O(1) if you remove from the end.  O(N) otherwise.
    */
    void RemoveAt(GUINT32 indx)
    {
        const GUINT32 len( Length() );

        if(len == 0 || indx >= len)
            THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);

        T *const targ( m_begin + indx );

        // Call the destructor on the item
        targ->~T();

        // Copy all the following items over 1
        if(indx < (len - 1))
        {
            if(IsMovableType<T>::Value)
                memmove(targ, targ + 1, (len - indx - 1) * sizeof(T));
            else
            {
                // If T is not a primitive type then this is potentially really expensive!!
                T *cur( targ );

                // Have to call the constructor on the first item, because we already destructed
                //  this memory location above
                new(cur) T(*(targ + 1));
                ++cur;

                // Then for the rest of the move, we use the assignment operator, because those
                //  items have already been initialized.
                GUINT32 i(indx + 1);
                for(; i < (len - 1); ++i, ++cur)
                    *cur = *(cur + 1);

                // And on the item at the end we call the destructor
                (m_begin + i)->~T();
            }
        }

        _set_length(len - 1);
    }

    /** Removes the first instance of the item.  The type T must have a comparison operator.
        \note O(n)
    */
    inline void RemoveOne(const T &o)
    {
        T *cur(m_begin);
        for(GUINT32 i(0); i < Length(); ++i, ++cur)
        {
            if(o == *cur)
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
        T *cur(m_begin + Length() - 1);
        for(GUINT32 i(0); i < Length(); ++i, --cur)
        {
            if(o == *cur)
            {
                RemoveAt(Length() - i - 1);
                break;
            }
        }
    }

    /** Removes all instances of the item.  The type T must have a comparison operator.
        \note O(n)
    */
    inline void RemoveAll(const T &o)
    {
        T *cur(m_begin + Length() - 1);
        for(GUINT32 i(0); i < Length(); ++i, --cur)
        {
            if(o == *cur)
                RemoveAt(Length() - i - 1);
        }
    }

    /** Pushes the item on the back of the list. */
    inline void PushBack(const T &o){ Insert(o, Length()); }
    /** Removes the item on the back of the list. */
    inline void PopBack(){ RemoveAt(Length() - 1); }

    /** Reserves room for at least this many items.
        Pass 0 to clear memory.
    */
    void Reserve(GUINT32 n)
    {
        const GUINT32 len( Length() );
        GUINT32 new_capacity( _capacity(n) );
        GUINT32 cur_capacity( Capacity() );
        if(new_capacity < cur_capacity)
        {
            // Need to call the destructors on the items we're (potentially) deleting
            if(new_capacity < len)
            {
                for(GUINT32 i(new_capacity); i < len; ++i)
                    m_begin[i].~T();
                _set_length( new_capacity );
            }
        }
        else if(new_capacity == cur_capacity)
            // No need to reallocate
            return;

        GUINT32 new_size_in_bytes(new_capacity * sizeof(T));
        if(new_capacity > 0)
            new_size_in_bytes += 2 * sizeof(GUINT32);

        void *real_begin(m_begin);
        if(m_begin)
            real_begin = reinterpret_cast<GUINT32 *>(real_begin) - 2;

        if(IsMovableType<T>::Value)
        {
            // As an optimization for primitive types (ones that are not affected by binary moves)
            //  we call realloc, because a hidden memory relocation doesn't affect our type.
            real_begin = realloc(real_begin, new_size_in_bytes);

            if(new_size_in_bytes > 0 && real_begin == NULL)
                THROW_NEW_GUTIL_EXCEPTION(BadAllocationException);

            if(real_begin == NULL)
                m_begin = NULL;
            else
                m_begin = reinterpret_cast<T *>( reinterpret_cast<GUINT32 *>(real_begin) + 2 );
        }
        else
        {
            if(new_size_in_bytes > 0)
            {
                // Have to manually reallocate and call the copy constructors, because a complex
                //  type may be dependent on their memory locations (self-pointers are one example)
                T *backup( m_begin );
                const GUINT32 len( Length() );
                void *new_begin( malloc(new_size_in_bytes) );

                if(new_begin == NULL)
                    THROW_NEW_GUTIL_EXCEPTION(BadAllocationException);

                T *cur( m_begin = reinterpret_cast<T *>( reinterpret_cast<GUINT32 *>(new_begin) + 2) );
                if(backup)
                {
                    for(GUINT32 i(0); i < len; ++i)
                        new(cur++) T(*(backup++));
                }
            }

            if(real_begin)
                free(real_begin);
            if(new_size_in_bytes == 0)
                m_begin = NULL;
        }

        if(new_capacity > 0)
            _set_capacity( new_capacity );
        if(cur_capacity == 0)
            _set_length( len );
    }

    /** Resizes the vector.  If the new size is larger than the current, then the default
        object will be copied to give a value to the newly created objects.
    */
    void Resize(GUINT32 new_size, const T &default_object = T()){
        if(new_size > Capacity())
            Reserve(new_size);
        if(new_size < Length())
        {
            const Vector<T>::iterator iter( begin() + new_size );
            for(int i(Length()); i > new_size; --i)
                Remove(iter);
        }
        else if(new_size > Length())
        {
            T *cur(m_begin + Length());
            for(int i(Length()); i < new_size; ++i)
                new(cur++) T(default_object);
        }
    }

    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline const T &operator [](GUINT32 i) const{ return m_begin[i]; }
    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline T &operator [](GUINT32 i){ return m_begin[i]; }

    /** Returns a pointer to the item at the given offset. */
    inline const T *operator +(GUINT32 offset) const{ return m_begin + offset; }
    /** Returns a pointer to the item at the given offset. */
    inline T *operator +(GUINT32 offset){ return m_begin + offset; }

    /** Accesses the data at the given index.
        \note Checks the index and throws an exception if it is out of bounds
    */
    inline T &At(GUINT32 i) throw(GUtil::Core::IndexOutOfRangeException<false>){
        if(Length() == 0 || i >= Length())
            THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
        return m_begin[i];
    }
    /** Accesses the data at the given index.
        \note Checks the index and throws an exception if it is out of bounds
    */
    inline const T &At(GUINT32 i) const throw(GUtil::Core::IndexOutOfRangeException<false>){
        if(Length() == 0 || i >= Length())
            THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
        return m_begin[i];
    }

    /** Returns a pointer to the start of the array. */
    inline const T *ConstData() const{ return m_begin; }
    /** Returns a pointer to the start of the array. */
    inline T *Data(){ return m_begin; }

    /** Clears all items from the stack and frees all memory. */
    inline void Clear(){ Reserve(0); }

    /** The current length of the vector. */
    inline GUINT32 Length() const{ return Length(m_begin); }

    /** The current length of an array which was allocated by the vector.
        \note This only works because the Vector class stores the length of the
        array just before the 0 index.  So you can't pass just any old array into it.
    */
    static inline GUINT32 Length(T *vec){
        return vec ? *(reinterpret_cast<GUINT32 *>(vec) - 1) : 0;
    }

    /** The current length of the vector. */
    inline GUINT32 Size() const{ return Length(); }
    /** The current length of the vector. */
    inline GUINT32 Count() const{ return Length(); }

    /** How many items of type T we are capable of holding. */
    inline GUINT32 Capacity() const{
        return m_begin ? *(reinterpret_cast<GUINT32 *>(m_begin) - 2) : 0;
    }

    /** Iterates through the vector. */
    class iterator
    {
        friend class Vector;
    public:
        inline iterator()
            :current(0),
              m_begin(0),
              m_end(0)
        {}
        inline iterator(T *begin, int index)
            :current(begin + index),
              m_begin(begin),
              m_end(begin + Vector<T>::Length(begin))
        {}

        inline T &operator *(){ return *current; }
        inline const T &operator *() const{ return *current; }
        inline T *operator ->(){ return current; }
        inline T const*operator ->() const{ return current; }

        inline GUINT32 Index() const{ return current ? current - m_begin : 0; }

        inline iterator &operator ++(){ ++current; return *this; }
        inline iterator operator ++(int){ iterator ret(*this); ++current; return ret;}
        inline iterator &operator +=(int n){ current += n; return *this; }
        inline iterator operator +(int n) const{ iterator ret(*this); ret.current += n; return ret; }

        inline iterator &operator --(){ --current; return *this; }
        inline iterator operator --(int){ iterator ret(*this); --current; return ret;}
        inline iterator &operator -=(int n){ current -= n; return *this; }
        inline iterator operator -(int n) const{ iterator ret(*this); ret.current -= n; return ret;}

        /** Returns whether the iterator is valid. */
        inline operator bool() const{ return current && m_begin <= current && current < m_end; }


    protected:
        T *current;
    private:
        T *m_begin;
        T *m_end;
    };

    /** Iterates through the vector, but also guarantees that it won't modify the vector. */
    class const_iterator
    {
        friend class Vector;
    public:
        inline const_iterator()
            :current(0),
              m_begin(0),
              m_end(0)
        {}
        inline const_iterator(T *begin, int index)
            :current(begin + index),
              m_begin(begin),
              m_end(begin + Vector<T>::Length(begin))
        {}
        inline const_iterator(const const_iterator &iter)
            :current(iter.current),
              m_begin(iter.m_begin),
              m_end(iter.m_end)
        {}
        inline const_iterator(const iterator &iter)
            :current(iter.current),
              m_begin(iter.m_begin),
              m_end(iter.m_end)
        {}

        inline T &operator *(){ return *current; }
        inline const T &operator *() const{ return *current; }
        inline T *operator ->(){ return current; }
        inline T const*operator ->() const{ return current; }

        inline GUINT32 Index() const{ return current ? current - m_begin : 0; }

        inline const_iterator &operator ++(){ ++current; return *this; }
        inline const_iterator operator ++(int){ const_iterator ret(*this); ++current; return ret;}
        inline const_iterator &operator +=(int n){ current += n; return *this; }
        inline const_iterator operator +(int n) const{ const_iterator ret(*this); ret.current += n; return ret; }

        inline const_iterator &operator --(){ --current; return *this; }
        inline const_iterator operator --(int){ const_iterator ret(*this); --current; return ret;}
        inline const_iterator &operator -=(int n){ current -= n; return *this; }
        inline const_iterator operator -(int n) const{ const_iterator ret(*this); ret.current -= n; return ret;}

        /** Returns the distance between iterators (how many items in between). */
        inline int operator - (const const_iterator &iter) const{
            if(m_begin != iter.m_begin) return INT_MAX;
            return current - iter.current;
        }

        /** Returns whether the iterator is valid. */
        inline operator bool() const{ return current && m_begin <= current && current < m_end;; }


    protected:
        T *current;
    private:
        T *m_begin;
        T *m_end;
    };

    inline iterator begin(){ return iterator(m_begin, 0); }
    inline const_iterator begin() const{ return const_iterator(m_begin, 0); }
    inline iterator end(){ return iterator(m_begin, Length()); }
    inline const_iterator end() const{ return const_iterator(m_begin, Length()); }

    inline iterator rbegin(){ return iterator(m_begin, Length() - 1); }
    inline const_iterator rbegin() const{ return const_iterator(m_begin, Length() - 1); }
    inline iterator rend(){ return iterator(m_begin, -1); }
    inline const_iterator rend() const{ return const_iterator(m_begin, -1); }


private:

    T *m_begin;

    inline void _set_length(GUINT32 len){ *(reinterpret_cast<GUINT32 *>(m_begin) - 1) = len; }
    inline void _set_capacity(GUINT32 cap){ *(reinterpret_cast<GUINT32 *>(m_begin) - 2) = cap; }

    inline static int _capacity(int n){ return n <= 0 ? 0 : GEN_BITMASK_32( FSB32( n ) ); }

};


/** Provides an "attachable" stack interface to the vector. */
template<class T>class VectorStack : public Stack<T>
{
    GUTIL_DISABLE_COPY(VectorStack<T>)
public:

    /** Attaches the stack interface to the vector.
        We will not delete your vector; that is up to you, because you may attach multiple
        interfaces to the same vector.
    */
    inline VectorStack(Vector<T> *vec) :m_vector(vec), m_delete(false){}

    /** Creates a new Vector with a stack interface.
        Use this if you don't want to supply a pre-existing vector object.
    */
    inline VectorStack() :m_vector(new Vector<T>), m_delete(true){}

    inline ~VectorStack(){ if(m_delete) delete m_vector; }


    /** Satisfies the Stack abstract interface. */
    void Push(const T &i){ m_vector->PushBack(i); }

    /** Satisfies the Stack abstract interface. */
    void Pop(){ m_vector->PopBack(); }

    /** Satisfies the Stack abstract interface. */
    const T &Top() const{ return (*m_vector)[m_vector->Length() - 1]; }

    /** Satisfies the Stack abstract interface. */
    T &Top(){ return (*m_vector)[m_vector->Length() - 1]; }

    /** Satisfies the Stack abstract interface. */
    GUINT32 CountStackItems() const{ return m_vector->Length(); }

    /** Satisfies the Stack abstract interface. */
    void FlushStack(){ m_vector->Clear(); }


private:

    Vector<T> *m_vector;
    bool m_delete;

};


template<class T>class VectorQueue : public Queue<T>
{
    GUTIL_DISABLE_COPY(VectorQueue<T>)
public:

    /** Attaches the Queue interface to the vector.
        We will not delete your vector; that is up to you, because you may attach multiple
        interfaces to the same vector.
    */
    inline VectorQueue(Vector<T> *vec) :m_vector(vec), m_delete(false){}

    /** Creates a new Vector with a queue interface.
        Use this if you don't want to supply a pre-existing vector object.
    */
    inline VectorQueue() :m_vector(new Vector<T>), m_delete(true){}

    inline ~VectorQueue(){ if(m_delete) delete m_vector; }

    /** Satisfies the Queue abstract interface. */
    void Enqueue(const T &i){ m_vector->PushBack(i); }

    /** Satisfies the Queue abstract interface. */
    void Dequeue(){ m_vector->PopFront(); }

    /** Satisfies the Queue abstract interface. */
    T &Front(){ return (*m_vector)[0]; }

    /** Satisfies the Queue abstract interface. */
    const T &Front() const{ return (*m_vector)[0]; }

    /** Satisfies the Queue abstract interface. */
    GUINT32 CountQueueItems() const{ return m_vector->Length(); }

    /** Satisfies the Queue abstract interface. */
    void FlushQueue(){ m_vector->Clear(); }


private:

    Vector<T> *m_vector;
    bool m_delete;

};


template<class T>class VectorDeque : public Deque<T>
{
    GUTIL_DISABLE_COPY(VectorDeque<T>)
public:

    /** Attaches the Deque interface to the vector.
        We will not delete your vector; that is up to you, because you may attach multiple
        interfaces to the same vector.
    */
    inline VectorDeque(Vector<T> *vec) :m_vector(vec), m_delete(false){}

    /** Creates a new Vector with a queue interface.
        Use this if you don't want to supply a pre-existing vector object.
    */
    inline VectorDeque() :m_vector(new Vector<T>), m_delete(true){}

    inline ~VectorDeque(){ if(m_delete) delete m_vector; }

    /** Satisfies the Deque abstract interface. */
    void PushBack(const T &i){ m_vector->PushBack(i); }

    /** Satisfies the Deque abstract interface. */
    void PopBack(){ m_vector->PopBack(); }

    /** Satisfies the Deque abstract interface. */
    void PushFront(const T &i){ m_vector->PushFront(i); }

    /** Satisfies the Deque abstract interface. */
    void PopFront(){ m_vector->PopFront(); }

    /** Satisfies the Deque abstract interface. */
    T &Front(){ return (*m_vector)[0]; }

    /** Satisfies the Deque abstract interface. */
    const T &Front() const{ return (*m_vector)[0]; }

    /** Satisfies the Deque abstract interface. */
    T &Back(){ return (*m_vector)[m_vector->Length() - 1]; }

    /** Satisfies the Deque abstract interface. */
    const T &Back() const{ return (*m_vector)[m_vector->Length() - 1]; }

    /** Satisfies the Deque abstract interface. */
    GUINT32 CountDequeItems() const{ return m_vector->Length(); }

    /** Satisfies the Deque abstract interface. */
    void FlushDeque(){ m_vector->Clear(); }


private:

    Vector<T> *m_vector;
    bool m_delete;

};


template<class T>class VectorRandomAccessContainer : public RandomAccessContainer<T>
{
    GUTIL_DISABLE_COPY(VectorRandomAccessContainer<T>)
public:

    inline VectorRandomAccessContainer(Vector<T> *vec) :m_vector(vec), m_delete(false){}

    inline VectorRandomAccessContainer() :m_vector(new Vector<T>), m_delete(true){}

    inline ~VectorRandomAccessContainer(){ if(m_delete) delete m_vector; }

    /** Satisfies the RandomAccessContainer abstract interface. */
    T &At(GUINT32 i){ return (*m_vector)[i]; }

    /** Satisfies the RandomAccessContainer abstract interface. */
    const T &At(GUINT32 i) const{ return (*m_vector)[i]; }

    /** Satisfies the RandomAccessContainer abstract interface. */
    void InsertAt(const T &i, GUINT32 indx){ m_vector->Insert(i, indx); }

    /** Satisfies the RandomAccessContainer abstract interface. */
    void RemoveAt(GUINT32 indx){ m_vector->RemoveAt(indx); }

    /** Satisfies the RandomAccessContainer abstract interface. */
    GUINT32 CountContainerItems() const{ return m_vector->Length(); }

    /** Satisfies the RandomAccessContainer abstract interface. */
    void FlushContainer(){ m_vector->Clear(); }


private:

    Vector<T> *m_vector;
    bool m_delete;

};



GUTIL_END_CORE_NAMESPACE;


namespace GUtil
{

template<class T>struct IsMovableType< Core::DataObjects::Vector<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::VectorStack<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::VectorQueue<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::VectorDeque<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::VectorRandomAccessContainer<T> >{ enum{ Value = 1 }; };

}

#endif // GUTIL_VECTOR_P
