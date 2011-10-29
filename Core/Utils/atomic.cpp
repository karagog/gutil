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

#include "atomic.h"
GUTIL_USING_CORE_NAMESPACE(Utils);

bool Atomic::Increment(GINT32 *i)
{
    GBYTE ret;
    asm(
        "lock;"
        "incl (%0);"
        "setne %%bl;"
            :"=a" (i), "=b" (ret)
    );
    return ret;
}

bool Atomic::Decrement(GINT32 *i)
{
    GBYTE ret;
    asm(
        "lock; "
        "decl (%0);"
        "setne %%bl;"
            :"=a" (i), "=b" (ret)
    );
    return ret;
}

int Atomic::FetchAndAdd(int *i, int n)
{
    int ret;
    asm(
        "lock;"
        "xaddl %%ebx, (%%eax);"
            : "=b" (ret), "=a" (i)
            : "a" (i), "b" (n)
    );
    return ret;
}

int Atomic::AddAndFetch(int *i, int n)
{
    asm(
        "lock;"
        "addl %%ebx, (%%eax);"
            : "=a" (i)
            : "a" (i), "b" (n)
    );
    return *i;
}
