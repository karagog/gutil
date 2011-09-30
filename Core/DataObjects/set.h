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

#ifndef GUTIL_SET_H
#define GUTIL_SET_H

#include "Core/DataObjects/dlist.h"
#include "Core/DataObjects/binarysearchtree.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements an associative container of objects.

    It organizes them efficiently by using a BinarySearchTree.  Can be
    treated as a unique set or a multi-set, depending on if you use
    Insert() or InsertMulti().
*/
template<class T> class Set
{
public:

    /** Constructs an empty set. */
    inline Set()
        :data(typename BinarySearchTree<SimpleDList<T>, T>::Comparer(), &get_list_representative),
          m_size(0)
    {}

    /** Constructs an empty set with the given compare function. */
    inline Set(int (*cmp)(const T &, const T &))
        :data(cmp, &get_list_representative),
          m_size(0)
    {}

    /** Conducts a deep copy of the set.
        \note O(N log(N))
    */
    inline Set(const Set<T> &o)
        :data(o.data),
          m_size(o.m_size)
    {}
    inline Set<T> &operator = (const Set<T> &o){
        Clear();
        ::new(this) Set<T>(o);
        return *this;
    }


    /** Inserts the item into the set.  If the item is already present it will be overwritten.
        \note O(log(N))
    */
    inline void Insert(const T &i){ _insert(i, false); }

    /** Inserts the item into the set.  You can have duplicate items.
        \note O(log(N))
    */
    inline void InsertMulti(const T &i){ _insert(i, true);}

    /** Removes the last one of these that you inserted.
        \note O(log(N))
    */
    inline void RemoveOne(const T &i){ _remove(i, false); }

    /** Removes and deletes all of those items from the set, in the reverse order you inserted them (LIFO).
        \note O(log(N) + M), where M is the number of items that will be removed from the set.  If you do
        not have duplicate items then this is O(log(N)).
    */
    inline void RemoveAll(const T &i){ _remove(i, true); }

    /** Does the set contain at least 1 of these?
        \note O(log(N))
    */
    inline bool Contains(const T &i){ return data.Search(i); }

    /** Empties the set and cleans up all memory. */
    void Clear(){ data.Clear(); m_size = 0;}

    /** How many items in the set.
        \note O(1)
    */
    inline long Size() const{ return m_size; }

    /** Returns the number of times this item occurs in the set.
        \note O(log(N))
    */
    inline int Count(const T &i){
        const_iterator iter(data.Search(i), true);
        return iter ? iter.stack().Count() : 0;
    }


    /** Iterates through the set.  Items are traversed in order.
        Duplicate items will be traversed in the order you inserted them.
    */
    class iterator :
            public BinarySearchTree< SimpleDList<T>, T >::iterator
    {
        friend class Set;
    public:
        inline iterator(){}
        inline iterator(const iterator &iter)
            :BinarySearchTree< SimpleDList<T>, T >::iterator(iter),
              siter(iter.siter)
        {}

        inline T &operator *(){ return *siter; }
        inline T *operator ->(){ return &(*siter); }

    protected:

        inline iterator(const typename BinarySearchTree< SimpleDList<T>, T >::iterator &iter, bool forward_direction)
            :BinarySearchTree< SimpleDList<T>, T >::iterator(iter)
        {
            // Initialize our stack iterator
            if(*this)
            {
                if(forward_direction)
                    siter = stack().begin();
                else
                    siter = stack().rbegin();
            }
        }

        inline SimpleDList<T> &stack(){ return BinarySearchTree< SimpleDList<T>, T >::iterator::current->Data; }

    private:
        typename SimpleDList<T>::iterator siter;

        // Overridden from bst_p
        void advance(){
            if(siter){
                if(!++siter){
                    BinarySearchTree< SimpleDList<T>, T >::iterator::advance();
                    if(*this)
                        siter = stack().begin();
                }
            }
        }
        // Overridden from bst_p
        void retreat(){
            if(siter){
                if(!--siter){
                    BinarySearchTree< SimpleDList<T>, T >::iterator::retreat();
                    if(*this)
                        siter = stack().rbegin();
                }
            }
        }
    };

    /** Iterates through the set.  Items are traversed in order.
        Duplicate items will be traversed in the order you inserted them.
    */
    class const_iterator :
            public BinarySearchTree< SimpleDList<T>, T >::const_iterator
    {
        friend class Set;
    public:
        inline const_iterator(){}
        inline const_iterator(const const_iterator &iter)
            :BinarySearchTree< SimpleDList<T>, T >::const_iterator(iter),
              siter(iter.siter)
        {}
        inline const_iterator(const iterator &iter)
            :BinarySearchTree< SimpleDList<T>, T >::const_iterator(iter),
              siter(iter.siter)
        {}

        const T &operator *() const{ return *siter; }
        const T *operator ->() const{ return &(*siter); }

    protected:

        inline const_iterator(const typename BinarySearchTree< SimpleDList<T>, T >::const_iterator &iter, bool forward_direction)
            :BinarySearchTree< SimpleDList<T>, T >::const_iterator(iter)
        {
            // Initialize our stack iterator
            if(*this)
            {
                if(forward_direction)
                    siter = stack().begin();
                else
                    siter = stack().rbegin();
            }
        }

        inline SimpleDList<T> const&stack(){ return BinarySearchTree< SimpleDList<T>, T >::const_iterator::current->Data; }

    private:
        typename SimpleDList<T>::const_iterator siter;

        // Overridden from bst_p
        void advance(){
            if(siter){
                if(!++siter){
                    BinarySearchTree< SimpleDList<T>, T >::const_iterator::advance();
                    if(*this)
                        siter = stack().begin();
                }
            }
        }
        // Overridden from bst_p
        void retreat(){
            if(siter){
                if(!--siter){
                    BinarySearchTree< SimpleDList<T>, T >::const_iterator::retreat();
                    if(*this)
                        siter = stack().rbegin();
                }
            }
        }
    };

    /** Returns an iterator at the beginning of the set.  Items are traversed in order.
        \note O(1)
    */
    inline iterator begin(){ return iterator(data.begin(), true); }
    /** Returns an iterator at the beginning of the set.  Items are traversed in order.
        \note O(1)
    */
    inline const_iterator begin() const{ return const_iterator(data.begin(), true); }
    /** Returns an iterator at the end of the set.
        \note O(1)
    */
    inline iterator end(){ return iterator(data.end(), true); }
    /** Returns an iterator at the end of the set.
        \note O(1)
    */
    inline const_iterator end() const{ return const_iterator(data.end(), true); }

    inline iterator rbegin(){ return iterator(data.rbegin(), false); }
    inline const_iterator rbegin() const{ return const_iterator(data.rbegin(), false); }
    inline iterator rend(){ return iterator(data.rend(), false); }
    inline const_iterator rend() const{ return const_iterator(data.rend(), false); }


private:

    BinarySearchTree< SimpleDList<T>, T > data;
    GUINT32 m_size;

    void _insert(const T &, bool);
    void _remove(const T &, bool);

    static const T &get_list_representative(const SimpleDList<T> &dl){ return *dl.begin(); }

};





/** Otherwise a normal set, but by default it allows duplicates. */
template<class T>class MultiSet :
        public Set<T>
{
public:

    inline MultiSet(){}
    inline MultiSet(const Set<T> &o) :Set<T>(o){}

    /** Overridden to call InsertMulti. */
    inline void Insert(const T &i){ Set<T>::InsertMulti(i); }

};









template<class T>void Set<T>::_insert(const T &i, bool allow_multiples)
{
    Set<T>::iterator iter( data.Search(i), true );
    if(iter)
    {
        SimpleDList<T> &s(iter.stack());
        if(!allow_multiples)
        {
            m_size -= s.Count();
            s.Clear();
        }
        typename SimpleDList<T>::iterator top(s.begin());
        s.Insert(i, top);
    }
    else
    {
        data.Add(i);
    }
    ++m_size;
}

template<class T>void Set<T>::_remove(const T &i, bool all)
{
    Set<T>::iterator iter( data.Search(i), true );
    if(iter)
    {
        SimpleDList<T> &s(iter.stack());
        typename SimpleDList<T>::iterator top(s.begin());
        s.Remove(top);
        --m_size;
        int cnt( s.Count() );
        if(all || cnt == 0)
        {
            m_size -= cnt;
            data.Remove(iter);
        }
    }
}

GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_SET_H
