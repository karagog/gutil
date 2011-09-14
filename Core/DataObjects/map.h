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
#include "Core/DataObjects/private/flexible_type_comparer.h"
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
        friend class Map;
    public:
        inline explicit Page(const K &k, const V &v, Map *m)
            :Key(k),
              values(m, &Key)
        {
            values.Push(v);
        }

        /** The key of this mapping. */
        K Key;

        /** Returns the last inserted value for this key, in the case InsertMulti() was used. */
        inline V &Value(){ return values.Top(); }
        /** Returns the last inserted value for this key, in the case InsertMulti() was used. */
        inline const V &Value() const{ return values.Top(); }

        /** Returns the stack of values associated with Key.
            \note This returns a non-const reference to the stack, thus allowing you to
            push/pop or do whatever to it.  The map is implemented such that you can
            safely pop the stack, and when the last item is popped the key will be
            removed from the map.  That operation will invalidate any iterator you
            had pointing to this page.
        */
        inline Stack<V> &Values(){ return values; }
        /** Returns the stack of values associated with Key */
        inline const Stack<V> &Values() const{ return values; }

    private:

        class value_stack :
                public Stack<V>
        {
        public:
            inline value_stack(Map *m, K *key)
                :m_map(m),
                  m_key(key)
            {}
        private:
            void on_popped(){
                // Remove the key if the last item was popped
                if(Stack<V>::Count() == 0)
                {
                    m_map->Remove(*m_key);
                }
            }
            Map *m_map;
            K *m_key;
        } values;

    };

public:

    /** Constructs an empty map with the default compare function. */
    inline Map()
        :_index(new PageWrapper),
          _key_searcher()
    {}

    /** Constructs an empty map with the specified key compare function.

        Errors in the compare function will result in errors in the implementation.
        You must have a well-defined ordering between items.

        \param cmp A compare function to use when comparing keys.  Return -1
        if lhs < rhs, return 1 if rhs < lhs and 0 if they're equal.
    */
    Map(int (*cmp)(const K &lhs, const K &rhs))
        :_index(new PageWrapper(cmp)),
          _key_searcher(cmp)
    {}


    /** Iterates through the key/value pairs. */
    class iterator :
            public BinarySearchTree<Page *>::const_iterator
    {
    public:
        iterator(){}
        iterator(const typename BinarySearchTree<Page *>::const_iterator &o)
            :BinarySearchTree<Page *>::const_iterator(o)
        {}

        Page *operator ->(){ return *reinterpret_cast<Page **>(BinarySearchTree<Page *>::const_iterator::current->Data); }
    };

    /** Iterates through the key/value pairs, but doesn't let you edit them.
        Use this over the regular iterator, when you can.
    */
    class const_iterator :
            public BinarySearchTree<Page *>::const_iterator
    {
    public:
        const_iterator(){}
        const_iterator(const typename BinarySearchTree<Page *>::const_iterator &o)
            :BinarySearchTree<Page *>::const_iterator(o)
        {}

        const Page *operator ->() const{ return *reinterpret_cast<const Page *const*>(BinarySearchTree<Page *>::const_iterator::current->Data); }
    };

    inline iterator begin(){ return _index.begin(); }
    inline const_iterator begin() const{ return _index.begin(); }
    inline iterator end(){ return _index.end(); }
    inline const_iterator end() const{ return _index.end(); }
    inline iterator preBegin(){ return _index.preBegin(); }
    inline const_iterator preBegin() const{ return _index.preBegin(); }

    inline iterator Search(const K &k){ return _index.Search(k, &_key_searcher); }
    inline const_iterator Search(const K &k) const{ return _index.Search(k, &_key_searcher); }


    /** Returns whether the map contains this key. */
    inline bool Contains(const K &k) const{ return _index.Search(k, &_key_searcher); }

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

    /** Returns the stack of values corresponding to the key.
        Iterating through the stack will go through the values in the opposite
        order in which you inserted them.
    */
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
        _index.Remove(_index.Search(k, &_key_searcher));
    }

    /** Removes all values in the map and cleans up memory.
        \note O(N)
    */
    inline void Clear(){ _index.Clear(); }


private:

    /** A wrapper class to conduct comparisons and memory allocation/deallocation */
    class PageWrapper :
            public BinarySearchTree<Page *>::TypeWrapper,
            public FlexibleTypeComparer<K>
    {
    public:
        inline PageWrapper(){}
        inline PageWrapper(int (*compare_function)(const K &, const K &))
            :FlexibleTypeComparer<K>(compare_function){}
        void Delete(Page **p) const{
            delete *p;
            BinarySearchTree<Page *>::TypeWrapper::Delete(p);
        }
    private:
        int CompareVoid(const void *const lhs, const void *const rhs) const{
            return FlexibleTypeComparer<K>::Compare((*reinterpret_cast<const Page *const *>(lhs))->Key,
                                                    (*reinterpret_cast<const Page *const *>(rhs))->Key);
        }
    };

    /** A void wrapper which compares a page and a key (so we don't have to search
        with a page).
    */
    class KeyWrapper :
            public Interfaces::IVoidComparer,
            public FlexibleTypeComparer<K>
    {
    public:
        inline KeyWrapper(){}
        inline KeyWrapper(int (*compare_function)(const K &, const K &))
            :FlexibleTypeComparer<K>(compare_function){}
        int CompareVoid(const void *const lhs, const void *const rhs) const{
            return Compare((*reinterpret_cast<const Page *const *>(lhs))->Key,
                                                    *reinterpret_cast<const K *const>(rhs));
        }
    };

    void _insert(const K &, const V &, bool);

    BinarySearchTree<Page *> _index;
    KeyWrapper _key_searcher;

};


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
    return iter->values.Top();
}

template<class K, class V>const Stack<V> &Map<K, V>::Values(const K &k) const
{
    iterator iter(_index.Search(k, &_key_searcher));
    if(!iter)
        THROW_NEW_GUTIL_EXCEPTION(GUtil::Core::IndexOutOfRangeException);
    return iter->values;
}

template<class K, class V>void Map<K, V>::_insert(const K &key, const V &value, bool overwrite)
{
    iterator iter(_index.Search(key, &_key_searcher));
    if(iter)
    {
        if(overwrite)
            iter->values.Clear();
        iter->values.Push(value);
    }
    else
    {
        _index.Add(new Page(key, value, this));
    }
}


GUTIL_END_CORE_NAMESPACE;

#endif // MAP_H
