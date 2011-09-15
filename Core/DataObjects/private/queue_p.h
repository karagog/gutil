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

#ifndef QUEUE_P_H
#define QUEUE_P_H

#include "nodechain.h"
#include "Core/Interfaces/ivoidwrappers.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a FIFO queue.
    A queue allows pushes and pops from both ends, and bidirectional iteration.
*/
class queue_p :
        public bidirectional_node_link
{
    GUTIL_DISABLE_COPY(queue_p);
public:

    long Size() const;
    inline long Length() const{ return Size(); }
    inline long Count() const{ return Size(); }

    void Clear();


protected:

    class queue_type_wrapper :
            public Interfaces::IVoidCopyer,
            public Interfaces::IVoidDeleter
    {
    public:
        virtual ~queue_type_wrapper(){}
    };

    queue_p(queue_type_wrapper *);
    inline ~queue_p(){ Clear(); delete data_wrapper; }

    void push_front(const void *const);
    void push_back(const void *const);
    void pop_front();
    void pop_back();

    void insert(const void *const, bidirectional_node_iterator iter);
    void remove(bidirectional_node_iterator);


private:

    queue_type_wrapper *data_wrapper;
    long m_size;

};


GUTIL_END_CORE_NAMESPACE;

#endif // QUEUE_P_H
