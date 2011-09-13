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

#ifndef SET_H
#define SET_H

#include "Core/DataObjects/private/bst_p.h"
#include "Core/DataObjects/stack.h"
#include "Core/DataObjects/private/flexible_type_comparer.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


template<class T> class Set :
        public bst_p
{
    GUTIL_DISABLE_COPY(Set<T>);
public:

    class TypeWrapper :
            public bst_p::void_wrapper,
            public FlexibleTypeComparer<T>
    {
    public:
        virtual int Compare(const Stack<T> &lhs, const Stack<T> &rhs){
            return FlexibleTypeComparer<T>::Compare(lhs.Top(), rhs.Top());
        }
        virtual Stack<T> *Copy(const Stack<T> &s){
            return s.CloneTo(*(new Stack<T>));
        }
        virtual void Delete(Stack<T> *s){
            delete s;
        }
        virtual ~TypeWrapper(){}

    private:
        int CompareVoid(const void *const lhs, const void *const rhs) const{
            return Compare(*reinterpret_cast<const Stack<T> *const>(lhs),
                           *reinterpret_cast<const Stack<T> *const>(rhs));
        }
        void *CopyVoid(const void *const v) const{
            return Copy(*reinterpret_cast<const Stack<T> *const>(v));
        }
        void DeleteVoid(void *) const{
            Delete(reinterpret_cast<Stack<T> *>(v));
        }
    };

    class SearchWrapper :
            public Interfaces::IVoidComparer,
            public Comparer
    {
        int CompareVoid(const void *const lhs, const void *const rhs) const{
            return compare_types(reinterpret_cast<const Stack<T> *const>(v)->Top(),
                                 *reinterpret_cast<const T *const>(rhs));
        }
    };

    Set()
        :data_wrapper(new TypeWrapper){}
    Set(int (*compare)(const T &, const T &))
        :data_wrapper(new TypeWrapper){}
    ~Set(){
        delete data_wrapper;
    }

protected:

    Interfaces::IVoidComparer *cmp;


private:

    TypeWrapper *data_wrapper;

};


GUTIL_END_CORE_NAMESPACE;

#endif // SET_H
