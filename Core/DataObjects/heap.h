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

#ifndef HEAP_H
#define HEAP_H

#include "Core/DataObjects/private/binary_tree_node.h"
#include "private/heap_p.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


template<class T>class Heap :
        public heap_p
{
public:

    /** An abstract type wrapper around the type.  It is abstract because you must implement
        the compare function.
    */
    class TypeWrapper :
            public heap_p::type_wrapper
    {
    public:
        virtual int Compare(const T &, const T &) = 0;
        virtual T *Copy(const T &o){
            return new T(o);
        }
        virtual void Delete(T *o){
            delete o;
        }

    private:
        virtual int CompareVoid(const void *const lhs, const void *const rhs) const{
            return Compare(*reinterpret_cast<const T *const>(lhs),
                           *reinterpret_cast<const T *const>(rhs));
        }
        virtual void *CopyVoid(const void *const v) const{
            return Copy(reinterpret_cast<const T *const>(v));
        }
        virtual void DeleteVoid(void *v) const{
            Delete(reinterpret_cast<T *>(v));
        }
    };

    Heap();

};


GUTIL_END_CORE_NAMESPACE;

#endif // HEAP_H
