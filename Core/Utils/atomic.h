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

#ifndef GUTIL_ATOMIC_H
#define GUTIL_ATOMIC_H

#include "gutil_macros.h"

GUTIL_BEGIN_CORE_NAMESPACE(Utils);


class GAtomic
{
public:

    static inline int FetchAndAdd(int *i, int n){
        int ret;
        asm(

        "movl , %ecx \n"
        "lock \n"
        "incl (%ecx) \n"
        "mov $0,%eax \n"
        "setne %al \n"
        "ret \n"
            :"=r" (ret)
            :"r" (i)
        );
    }

};


GUTIL_END_CORE_NAMESPACE

#endif // GUTIL_ATOMIC_H