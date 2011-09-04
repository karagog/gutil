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
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a templated mapping between values.

    The key is indexed with a binary search tree, for O(log(N)) lookups.
*/
template<class K, class V>class Map
{
public:

    Map();

    void Insert(const K &key, const V &value){
        BinarySearchTree<Page *>::const_iterator iter(_index.Search(key));
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

    void InsertMulti(const K &key, const V &value){
        BinarySearchTree<Page *>::const_iterator iter(_index.Search(key));
        if(iter)
            iter->Values.Push(value);
        else
            _index.Add(new Page(key, value));
    }


private:

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

    /** A wrapper class to conduct comparisons and memory allocation/deallocation */
    class TypeWrapper :
            public BinarySearchTree<Page *>::TypeWrapper
    {
    public:
        int Compare(Page *const&lhs, Page *const&rhs) const{
            const K &key1(lhs->Key);
            const K &key2(rhs->Key);
            if(key1 < key2)
                return -1;
            else if(key2 < key1)
                return 1;
            return 0;
        }
    };

    BinarySearchTree<Page *> _index;

};


template<class K, class V>Map::Map()
    :_index(new TypeWrapper)
{}



GUTIL_END_CORE_NAMESPACE;

#endif // MAP_H
