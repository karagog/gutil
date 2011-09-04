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
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a templated mapping between values.

    The key is indexed with a binary search tree, for O(log(N)) lookups.
*/
template<class K, class V>class Map
{
public:


private:

    class Page
    {
    public:
        Page(const K &k, const V &v)
            :Key(k)
        {
            Values.Push(v);
        }
        Page(const Page &p)
            :Key(p.Key)
        {
            // List<V> lst;
            for(Stack<V>::const_iterator iter(p.Values.begin()); iter != p.Values.end(); iter++)
                lst.PushFront(*iter);
            for(List<V>::const_iterator iter(lst.begin()); iter != lst.end(); iter++)
                Values.Push(*iter);
        }

        K Key;
        Stack<V> Values;
    };

};


GUTIL_END_CORE_NAMESPACE;

#endif // MAP_H
