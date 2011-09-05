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

    Map();

    class iterator :
            public BinarySearchTree<Page *>::const_iterator
    {
    public:
        iterator(){}
        iterator(const typename BinarySearchTree<Page *>::const_iterator &o)
            :BinarySearchTree<Page *>::const_iterator(o)
        {}

        Page &operator*(){ return **(reinterpret_cast<Page **>(this->current->Data)); }
        Page *operator ->(){ return *reinterpret_cast<Page **>(this->current->Data); }
    };

    const V &At(const K &) const;
    V &At(const K &);

    const Stack<V> &Values(const K &) const;

    void Insert(const K &key, const V &value);
    void InsertMulti(const K &key, const V &value);


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

    /** A type wrapper which compares a page and a key (so we don't have to search
        with a page).
    */
    class KeyWrapper :
            public BinarySearchTree<Page *>::TypeWrapper,
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


template<class K, class V>const V &Map<K, V>::At(const K &k) const
{
    typename BinarySearchTree<Page *>::const_iterator iter(_index.Search(k, &_key_searcher));
    if(!iter)
        THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
    return *iter->Values.Top();
}

template<class K, class V>V &Map<K, V>::At(const K &k)
{
    iterator iter(_index.Search(k, &_key_searcher));
    if(!iter)
        THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
    return *(iter->Values.Top());
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


GUTIL_END_CORE_NAMESPACE;

#endif // MAP_H
