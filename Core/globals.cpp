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

#include "gutil.h"


const char MSB_LOOKUP_TABLE[256] =
{
    #define _MSB_LT2(n) n, n
    #define _MSB_LT3(n) _MSB_LT2(n), _MSB_LT2(n)
    #define _MSB_LT4(n) _MSB_LT3(n), _MSB_LT3(n)
    #define _MSB_LT5(n) _MSB_LT4(n), _MSB_LT4(n)
    #define _MSB_LT6(n) _MSB_LT5(n), _MSB_LT5(n)
    #define _MSB_LT7(n) _MSB_LT6(n), _MSB_LT6(n)
    #define _MSB_LT8(n) _MSB_LT7(n), _MSB_LT7(n)

    -1, 0,
    _MSB_LT2(1),
    _MSB_LT3(2),
    _MSB_LT4(3),
    _MSB_LT5(4),
    _MSB_LT6(5),
    _MSB_LT7(6),
    _MSB_LT8(7)
};


GUTIL_COM_EXPORT int MSB32(unsigned int n)
{
    int res;
    unsigned int t1, t2;
    if((t1 = n >> 16))
    {
      res = (t2 = t1 >> 8) ? 24 + MSB_LOOKUP_TABLE[t2] :
                           16 + MSB_LOOKUP_TABLE[t1];
    }
    else
    {
      res = (t2 = n >> 8) ? 8 + MSB_LOOKUP_TABLE[t2] :
                          MSB_LOOKUP_TABLE[n];
    }
    return res;
}

GUTIL_COM_EXPORT int MSB64(GUINT64 n)
{
    int res;
    GUINT64 t1, t2;
    if((t2 = n >> 32))
    {
        if((t1 = t2 >> 16))
        {
            res = (t2 = t1 >> 8) ? 56 +  MSB_LOOKUP_TABLE[t2] :
                                   48 + MSB_LOOKUP_TABLE[t1];
        }
        else
        {
            res = (t2 = t1 >> 8) ? 40 +  MSB_LOOKUP_TABLE[t2] :
                                   32 + MSB_LOOKUP_TABLE[t1];
        }
    }
    else if((t1 = n >> 16))
    {
      res = (t2 = t1 >> 8) ? 24 + MSB_LOOKUP_TABLE[t2] :
                           16 + MSB_LOOKUP_TABLE[t1];
    }
    else
    {
      res = (t2 = n >> 8) ? 8 + MSB_LOOKUP_TABLE[t2] :
                          MSB_LOOKUP_TABLE[n];
    }
    return res;
}
