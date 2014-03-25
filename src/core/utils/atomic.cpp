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

#include "atomic.h"

NAMESPACE_GUTIL;


bool AtomicInt::Increment()
{
    GBYTE ret;
    asm(
        "lock;"
        "incl %0;"
        "setne %1"
            :"=m" (m_value), "=m" (ret)
            :"m" (m_value)
    );
    return ret;
}

bool AtomicInt::Decrement()
{
    GBYTE ret;
    asm(
        "lock; "
        "decl %0;"
        "setne %1;"
            :"=m" (m_value), "=m" (ret)
            :"m" (m_value)
    );
    return ret;
}

int AtomicInt::FetchAndAdd(int n)
{
    asm(
        "lock;"
        "xaddl %0, %1;"
            :"=r" (n), "+m" (m_value)
            :"0" (n)
        );
    return n;
}

int AtomicInt::AddAndFetch(int n)
{
    asm(
        "lock;"
        "addl %0, %1;"
            :"=r"(n), "+m" (m_value)
            :"0" (n)
    );
    return n;
}


END_NAMESPACE_GUTIL;
