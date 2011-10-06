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

#include "Core/DataObjects/vector.h"
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
        friend class Map;
    public:
        inline explicit Page(const K &k, const V &val)
            :Key(k), values(val, 1)
        {}

        inline Page(const Page &o)
            :Key(o.Key), values(o.values)
        {}

        /** The key of this mapping. */
        K Key;

        /** Returns the last inserted value for this key, in the case InsertMulti() was used. */
        inline V &Value(){ return values.Top(); }
        /** Returns the last inserted value for this key, in the case InsertMulti() was used. */
        inline const V &Value() const{ return values.Top(); }

        /** Returns the stack of values associated with Key.
            You cannot modify the stack, but you can modify the values in it.
        */
        inline const Stack<V> &Values() const{ return values; }

    protected:

        Vector<V> values;

    };

public:

    /** Constructs an empty map with the default compare function. */
    inline Map()
        :_index(&get_key_value)
    {}

    /** Constructs an empty map with the specified key compare function.

        Errors in the compare function will result in errors in the implementation.
        You must have a well-defined ordering between items.

        \param cmp A compare function to use when comparing keys.  Return -1
        if lhs < rhs, return 1 if rhs < lhs and 0 if they're equal.
    */
    inline Map(int (*cmp)(const K &lhs, const K &rhs))
        :_index(cmp, &get_key_value)
    {}

    Map(const Map<K, V> &o)
        :_index(o._index)
    {}


    /** Iterates through the key/value pairs. */
    class iterator :
            public BinarySearchTree<Page, K>::iterator
    {
    public:
        inline iterator(){}
        inline iterator(const typename BinarySearchTree<Page, K>::iterator &o)
            :BinarySearchTree<Page, K>::iterator(o)
        {}
    };

    /** Iterates through the key/value pairs.
        Guarantees that you won't modify the map, but you can still modify the values.
    */
    class const_iterator :
            public BinarySearchTree<Page, K>::const_iterator
    {
    public:
        inline const_iterator(){}
        inline const_iterator(const typename BinarySearchTree<Page, K>::const_iterator &o)
            :BinarySearchTree<Page, K>::const_iterator(o)
        {}
    };

    inline iterator begin(){ return _index.begin(); }
    inline const_iterator begin() const{ return _index.begin(); }
    inline iterator end(){ return _index.end(); }
    inline const_iterator end() const{ return _index.end(); }
    inline iterator preBegin(){ return _index.preBegin(); }
    inline const_iterator preBegin() const{ return _index.preBegin(); }

    inline iterator Search(const K &k){ return _index.Search(k); }
    inline const_iterator Search(const K &k) const{ return _index.Search(k); }


    /** Returns whether the map contains this key. */
    inline bool Contains(const K &k) const{ return _index.Search(k); }

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
    inline V &operator [](const K &k){ return At(k); }

    /** Returns the stack of values corresponding to the key. */
    const Stack<V> &Values(const K &) const;

    /** Inserts an item into the map.

        If a value (or values) already exists for that key then they will be overwritten.
        \returns An iterator pointing to the inserted item
    */
    inline void Insert(const K &key, const V &value){ _insert(key, value, true); }

    /** Inserts an item into the map.

        If a value already exists for that key then the new value will be added to the
        collection of values corresponding to that key.
        \returns An iterator pointing to the inserted item
    */
    inline void InsertMulti(const K &key, const V &value){ _insert(key, value, false); }

    /** Removes all values corresponding to the key. */
    inline void Remove(const K &k){
        _index.Remove(_index.Search(k));
    }

    /** Removes all values in the map and cleans up memory.
        \note O(N)
    */
    inline void Clear(){ _index.Clear(); }


private:

    BinarySearchTree<Page, K> _index;

    void _insert(const K &, const V &, bool);

    static const K &get_key_value(const Page &p){ return p.Key; }

};


template<class K, class V>const V &Map<K, V>::At(const K &k) const
{
    typename BinarySearchTree<Page, K>::iterator iter(_index.Search(k));
    if(!iter)
        THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
    return iter->Value();
}

template<class K, class V>V &Map<K, V>::At(const K &k)
{
    iterator iter(_index.Search(k));
    if(!iter)
        THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
    return iter->Value();
}

template<class K, class V>const Stack<V> &Map<K, V>::Values(const K &k) const
{
    iterator iter(_index.Search(k));
    if(!iter) THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
    return iter->Values();
}

template<class K, class V>void Map<K, V>::_insert(const K &key, const V &value, bool overwrite)
{
    iterator iter(_index.Search(key));
    if(iter)
    {
        if(overwrite)
            iter->values.FlushStack();
        iter->values.Push(value);
    }
    else
    {
        _index.Add(Page(key, value));
    }
}


GUTIL_END_CORE_NAMESPACE;

#endif // MAP_H
