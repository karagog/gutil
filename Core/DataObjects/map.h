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

#ifndef GUTIL_MAP_H
#define GUTIL_MAP_H

#include "gutil_vector.h"
#include "gutil_binarysearchtree.h"
NAMESPACE_GUTIL1(DataObjects);


/** Implements a templated mapping between values.

    The key is indexed with a binary search tree, for O(log(N)) lookups.
*/
template<class K, class V>class Map
{
public:

    /** Describes one mapping of a key to a stack of values. */
    class Page
    {
        friend class Map;
    public:
        inline explicit Page(const K &k, const V &val)
            :key(k), values(val, 1)
        {}

        inline Page(const Page &o)
            :key(o.key), values(o.values)
        {}

        /** The key of this mapping. */
        inline const K &Key() const{ return key; }
        /** The key of this mapping. */
        inline K &Key(){ return key; }

        /** Returns the last inserted value for this key, in the case InsertMulti() was used. */
        inline V &Value(){ return values[values.Size() - 1]; }
        /** Returns the last inserted value for this key, in the case InsertMulti() was used. */
        inline const V &Value() const{ return values[values.Size() - 1]; }

        /** Returns the stack of values associated with Key.
            You cannot modify the stack, but you can modify the values in it.
        */
        inline const Vector<V> &Values() const{ return values; }

    protected:

        K key;
        Vector<V> values;

    };
    

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

    inline Map(const Map<K, V> &o)
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
    inline int Size() const{ return _index.Size(); }

    /** Returns the value corresponding to the key.  If the key does not exist, it throws an
        IndexOutOfRangeException.
        If more than one value exist then you get the last one which was inserted.
        \sa Find()
    */
    const V &At(const K &) const;
    /** Returns the value corresponding to the key.  If the key does not exist, it throws an
        IndexOutOfRangeException.
        If more than one value exist then you get the last one which was inserted.
        \sa Find()
    */
    V &At(const K &);

    /** Returns a pointer to the value referenced by the key, or a NULL value if the key does not
        exist in the map.
        If more than one value exist then you get the last one which was inserted.
        \sa At()
    */
    V const *Find(const K &) const;
    /** Returns a pointer to the value referenced by the key, or a NULL value if the key does not
        exist in the map.
        If more than one value exist then you get the last one which was inserted.
        \sa At()
    */
    V *Find(const K &);

    /** Returns the value corresponding to the key.  If the key does not exist in the map, it
        will be inserted with a blank value.
        \sa At()
    */
    inline V &operator [](const K &k);

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

    inline Vector<K> Keys() const{
        Vector<K> ret(Size());
        for(const_iterator iter(begin()); iter != end(); ++iter)
            ret.PushBack(iter->Key());
        return ret;
    }


private:

    BinarySearchTree<Page, K> _index;

    void _insert(const K &, const V &, bool);

    static const K &get_key_value(const Page &p){ return p.Key(); }

};


template<class K, class V>const V &Map<K, V>::At(const K &k) const
{
    iterator iter(_index.Search(k));
    if(!iter)
        THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
    return iter->Value();
}

template<class K, class V>V &Map<K, V>::At(const K &k)
{
    iterator iter(_index.Search(k));
    if(!iter)
        THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
    return iter->Value();
}

template<class K, class V>V const *Map<K, V>::Find(const K &k) const
{
    iterator iter(_index.Search(k));
    return iter ? &iter->Value() : 0;
}

template<class K, class V>V *Map<K, V>::Find(const K &k)
{
    iterator iter(_index.Search(k));
    return iter ? &iter->Value() : 0;
}

template<class K, class V>V &Map<K, V>::operator [](const K &k)
{
    iterator iter(_index.Search(k));
    if(!iter)
    {
        Insert(k, V());
        iter = _index.Search(k);
    }
    return iter->Value();
}

template<class K, class V>const Stack<V> &Map<K, V>::Values(const K &k) const
{
    iterator iter(_index.Search(k));
    if(!iter) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
    return iter->Values();
}

template<class K, class V>void Map<K, V>::_insert(const K &key, const V &value, bool overwrite)
{
    iterator iter(_index.Search(key));
    if(iter)
    {
        if(overwrite)
            iter->values.Clear();
        iter->values.Insert(value, iter->values.Size());
    }
    else
    {
        _index.Add(Page(key, value));
    }
}


END_NAMESPACE_GUTIL1;


namespace GUtil
{

// The map is binary-movable only if the BinarySearchTree is
template<class T, class U>struct IsMovableType< DataObjects::Map<T, U> > :
        public IsMovableType< DataObjects::BinarySearchTree<T> >{};

}


#endif // GUTIL_MAP_H
