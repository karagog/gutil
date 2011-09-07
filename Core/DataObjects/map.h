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

#ifndef MAP_H
#define MAP_H

#include "Core/DataObjects/stack.h"
#include "binarysearchtree.h"
#include "Core/Interfaces/icomparer.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a templated mapping between values.

    The key is indexed with a binary search tree, for O(log(N)) lookups.
*/
template<class K, class V>class Map
{
    /** Describes one mapping of a key to a stack of values. */
    class Page
    {
        GUTIL_DISABLE_COPY(Page);
    public:
        explicit Page(const K &k, const V &v)
            :Key(k)
        {
            Values.Push(v);
        }

        K Key;
        Stack<V> Values;
    };

public:

    /** Constructs an empty map. */
    Map();
    /** Reclaims all memory. */
    ~Map();

    class iterator :
            public BinarySearchTree<Page *>::const_iterator
    {
    public:
        iterator(){}
        iterator(const typename BinarySearchTree<Page *>::const_iterator &o)
            :BinarySearchTree<Page *>::const_iterator(o)
        {}

        Page &operator*(){ return **(reinterpret_cast<Page **>(BinarySearchTree<Page *>::const_iterator::current->Data)); }
        Page *operator ->(){ return *reinterpret_cast<Page **>(BinarySearchTree<Page *>::const_iterator::current->Data); }
    };

    class const_iterator :
            public BinarySearchTree<Page *>::const_iterator
    {
    public:
        const_iterator(){}
        const_iterator(const typename BinarySearchTree<Page *>::const_iterator &o)
            :BinarySearchTree<Page *>::const_iterator(o)
        {}

        const Page &operator*() const{ return **(reinterpret_cast<const Page *const*>(BinarySearchTree<Page *>::const_iterator::current->Data)); }
        const Page *operator ->() const{ return *reinterpret_cast<const Page *const*>(BinarySearchTree<Page *>::const_iterator::current->Data); }
    };

    iterator begin(){ return _index.begin(); }
    const_iterator begin() const{ return _index.begin(); }
    iterator end(){ return _index.end(); }
    const_iterator end() const{ return _index.end(); }
    iterator preBegin(){ return _index.preBegin(); }
    const_iterator preBegin() const{ return _index.preBegin(); }


    /** Returns whether the map contains this key. */
    bool Contains(const K &) const;

    /** How many unique keys are in the map. */
    inline long Size() const{ return _index.Size(); }

    /** Returns the value corresponding to the key.
        If more than one value exist then you get the last one which was inserted.
    */
    const V &At(const K &) const;
    /** Returns the value corresponding to the key.
        If more than one value exist then you get the last one which was inserted.
    */
    V &At(const K &);

    /** Returns the value corresponding to the key.
        \sa At()
    */
    inline const V &operator [](const K &k) const{ return At(k); }
    /** Returns the value corresponding to the key.
        \sa At()
    */
    V &operator [](const K &k){ return At(k); }

    /** Returns the stack of values corresponding to the key.
        Iterating through the stack will go through the values in the opposite
        order in which you inserted them.
    */
    const Stack<V> &Values(const K &) const;

    /** Inserts an item into the map.

        If a value (or values) already exists for that key then they will be overwritten.
    */
    void Insert(const K &key, const V &value);

    /** Inserts an item into the map.

        If a value already exists for that key then the new value will be added to the
        collection of values corresponding to that key.
    */
    void InsertMulti(const K &key, const V &value);

    /** Removes all values corresponding to the key. */
    void Remove(const K &);


private:

    /** A base class to provide a common comparison function between Keys. */
    class KeyComparer
    {
    public:
        int CompareKeys(const K &lhs, const K &rhs) const{
            if(lhs < rhs)
                return -1;
            else if(rhs < lhs)
                return 1;
            return 0;
        }
    };

    /** A wrapper class to conduct comparisons and memory allocation/deallocation */
    class PageWrapper :
            public BinarySearchTree<Page *>::TypeWrapper,
            public KeyComparer
    {
    public:
        int Compare(Page *const&lhs, Page *const&rhs) const{
            return CompareKeys(lhs->Key, rhs->Key);
        }
    };

    /** A void wrapper which compares a page and a key (so we don't have to search
        with a page).
    */
    class KeyWrapper :
            public Interfaces::IVoidComparer,
            public KeyComparer
    {
    private:
        int CompareVoid(const void *const lhs, const void *const rhs) const{
            return CompareKeys((*reinterpret_cast<const Page *const *>(lhs))->Key,
                               *reinterpret_cast<const K *const>(rhs));
        }
    };

    BinarySearchTree<Page *> _index;
    KeyWrapper _key_searcher;

};


template<class K, class V>Map<K, V>::Map()
    :_index(new PageWrapper)
{}

template<class K, class V>Map<K, V>::~Map()
{
    // Have to delete all the pages in the index
    for(iterator iter(_index.begin()); iter; iter++)
        delete &(*iter);
}


template<class K, class V>bool Map<K, V>::Contains(const K &k) const
{
    return _index.Search(k, &_key_searcher);
}

template<class K, class V>const V &Map<K, V>::At(const K &k) const
{
    typename BinarySearchTree<Page *>::const_iterator iter(_index.Search(k, &_key_searcher));
    if(!iter)
        THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
    return iter->Values.Top();
}

template<class K, class V>V &Map<K, V>::At(const K &k)
{
    iterator iter(_index.Search(k, &_key_searcher));
    if(!iter)
        THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
    return iter->Values.Top();
}

template<class K, class V>const Stack<V> &Map<K, V>::Values(const K &k) const
{
    typename BinarySearchTree<Page *>::const_iterator iter(_index.Search(k, &_key_searcher));
    if(!iter)
        THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
    return iter->Values;
}

template<class K, class V>void Map<K, V>::Insert(const K &key, const V &value)
{
    iterator iter(_index.Search(key, &_key_searcher));
    if(iter)
    {
        iter->Values.Clear();
        iter->Values.Push(value);
    }
    else
    {
        _index.Add(new Page(key, value));
    }
}

template<class K, class V>void Map<K, V>::InsertMulti(const K &key, const V &value)
{
    typename BinarySearchTree<Page *>::const_iterator iter(_index.Search(key));
    if(iter)
        iter->Values.Push(value);
    else
        _index.Add(new Page(key, value));
}

template<class K, class V>void Map<K, V>::Remove(const K &k)
{
    iterator iter(_index.Search(k, &_key_searcher));
    if(iter)
    {
        delete &(*iter);
        _index.Remove(iter);
    }
}


GUTIL_END_CORE_NAMESPACE;

#endif // MAP_H
