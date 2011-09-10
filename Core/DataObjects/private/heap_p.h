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

#ifndef HEAP_P_H
#define HEAP_P_H

#include "binary_tree_node.h"
#include "Core/Interfaces/ivoidwrappers.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Represents a generic binary heap implementation. */
class heap_p
{
public:

    class type_wrapper :
            public Interfaces::IVoidComparer,
            public Interfaces::IVoidCopyer,
            public Interfaces::IVoidDeleter
    {};

    inline heap_p(type_wrapper *tw)
        :data_wrapper(tw)
    {}


private:

    type_wrapper *data_wrapper;
    binary_tree_node *root;

};


GUTIL_END_CORE_NAMESPACE;

#endif // HEAP_P_H
