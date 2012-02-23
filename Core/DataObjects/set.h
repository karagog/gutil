/*Copyright 2010-2012 George Karagoulis

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

#include "Core/DataObjects/vector.h"
#include "Core/DataObjects/binarysearchtree.h"
NAMESPACE_GUTIL1(DataObjects);


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
        :data(&get_list_representative),
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
        new(this) Set<T>(o);
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
    inline bool Contains(const T &i) const{ return data.Search(i); }

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
        return iter ? iter.values().Count() : 0;
    }


    /** Iterates through the set.  Items are traversed in order.
        Duplicate items will be traversed in the order you inserted them.
    */
    class const_iterator :
            public BinarySearchTree< Vector<T>, T >::const_iterator
    {
        friend class Set;
    public:
        inline const_iterator(){}
        inline const_iterator(const const_iterator &iter)
            :BinarySearchTree< Vector<T>, T >::const_iterator(iter),
              siter(iter.siter)
        {}

        const T &operator *() const{ return *siter; }
        const T *operator ->() const{ return &(*siter); }

    protected:

        inline const_iterator(const typename BinarySearchTree< Vector<T>, T >::const_iterator &iter, bool forward_direction)
            :BinarySearchTree< Vector<T>, T >::const_iterator(iter)
        {
            // Initialize our stack iterator
            if(*this)
            {
                if(forward_direction)
                    siter = values().begin();
                else
                    siter = values().rbegin();
            }
        }

        inline Vector<T> &values(){ return BinarySearchTree< Vector<T>, T >::const_iterator::current->Data; }

        /** Overridden from BST */
        virtual void advance(){
            if(siter){
                if(!++siter){
                    BinarySearchTree< Vector<T>, T >::const_iterator::advance();
                    if(*this)
                        siter = values().begin();
                }
            }
        }
        /** Overridden from BST */
        virtual void retreat(){
            if(siter){
                if(!--siter){
                    BinarySearchTree< Vector<T>, T >::const_iterator::retreat();
                    if(*this)
                        siter = values().rbegin();
                }
            }
        }


    private:

        typename Vector<T>::const_iterator siter;

    };

    /** Returns an iterator at the beginning of the set.  Items are traversed in order.
        \note O(1)
    */
    inline const_iterator begin() const{ return const_iterator(data.begin(), true); }

    /** Returns an iterator at the end of the set.
        \note O(1)
    */
    inline const_iterator end() const{ return const_iterator(data.end(), true); }

    /** Returns an iterator at the reverse-beginning of the set.  Items are traversed in reverse order.
        \note O(1)
    */
    inline const_iterator rbegin() const{ return const_iterator(data.rbegin(), false); }

    /** Returns an iterator at the reverse-end of the set.
        \note O(1)
    */
    inline const_iterator rend() const{ return const_iterator(data.rend(), false); }


private:

    BinarySearchTree< Vector<T>, T > data;
    GUINT32 m_size;

    void _insert(const T &, bool);
    void _remove(const T &, bool);

    static const T &get_list_representative(const Vector<T> &dl){ return *dl.begin(); }

};





/** Otherwise a normal set, but by default it allows duplicates. */
template<class T>class MultiSet :
        public Set<T>
{
public:

    inline MultiSet(){}
    inline MultiSet(const Set<T> &o) :Set<T>(o){}
    inline MultiSet(int (*cmp)(const T &, const T &)) :Set<T>(cmp){}

    /** Overridden to call InsertMulti. */
    inline void Insert(const T &i){ Set<T>::InsertMulti(i); }

};









template<class T>void Set<T>::_insert(const T &i, bool allow_multiples)
{
    Set<T>::const_iterator iter( data.Search(i), true );
    if(iter)
    {
        Vector<T> &v(iter.values());
        if(!allow_multiples)
        {
            m_size -= v.Count();
            v.Clear();
        }
        v.PushBack(i);
    }
    else
    {
        data.Add(Vector<T>(i));
    }
    ++m_size;
}

template<class T>void Set<T>::_remove(const T &i, bool all)
{
    Set<T>::const_iterator iter( data.Search(i), true );
    if(iter)
    {
        Vector<T> &v(iter.values());
        v.PopBack();
        --m_size;
        if(all || v.Count() == 0)
        {
            m_size -= v.Count();
            data.Remove(iter);
        }
    }
}

END_NAMESPACE_GUTIL1;


namespace GUtil
{

// The set is binary-movable only if the BinarySearchTree is
template<class T>struct IsMovableType< DataObjects::Set<T> > :
        public IsMovableType< DataObjects::BinarySearchTree<T> >{};

}


#endif // GUTIL_SET_H
