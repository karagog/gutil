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

#ifndef GUTIL_GASSERT_H
#define GUTIL_GASSERT_H

#include "gutil_globals.h"

#if defined(GUTIL_DEBUG)
    #include <stdio.h>
    #include <stdlib.h>
    #define GASSERT(b)          if((!b)){ printf("Assertion failed on line %d of %s", __LINE__, __FILE__); abort(); } while(0)
    #define GASSERT2(b, msg)    if((!b)){ printf("Assertion failed on line %d of %s: %s", __LINE__, __FILE__, msg); abort(); } while(0)
#else
    #define GASSERT(b)
    #define GASSERT2(b, msg)
#endif

#endif // GUTIL_GASSERT_H
