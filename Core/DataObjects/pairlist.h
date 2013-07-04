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

#ifndef GUTIL_PAIRLIST_H
#define GUTIL_PAIRLIST_H

#include "gutil_pair.h"
#include "gutil_list.h"

NAMESPACE_GUTIL1(DataObjects);


/** Describes a list of value pairs. */
template<class T1, class T2>
class PairList :
        public List< Pair<T1, T2> >
{
public:
    inline PairList(){}
    inline PairList(GUINT32 capacity) : List< Pair<T1, T2> >(capacity){}
};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_PAIRLIST_H
