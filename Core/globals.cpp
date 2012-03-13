/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "gutil_macros.h"
#include <cstdlib>
#include <time.h>

#if !defined(GUTIL_COM_EXPORTS) && !defined(GUTIL_COM_IMPORTS)
NAMESPACE_GUTIL;
#endif


/** A lookup table for quickly determining the first set bit of an integer */
static GBYTE const _MSB_LUT[] =
{
    #define _MSB_LT2(n) n, n
    #define _MSB_LT3(n) _MSB_LT2(n), _MSB_LT2(n)
    #define _MSB_LT4(n) _MSB_LT3(n), _MSB_LT3(n)
    #define _MSB_LT5(n) _MSB_LT4(n), _MSB_LT4(n)
    #define _MSB_LT6(n) _MSB_LT5(n), _MSB_LT5(n)
    #define _MSB_LT7(n) _MSB_LT6(n), _MSB_LT6(n)
    #define _MSB_LT8(n) _MSB_LT7(n), _MSB_LT7(n)

    -1, // Returns error if no bits are set
    0,
    _MSB_LT2(1),
    _MSB_LT3(2),
    _MSB_LT4(3),
    _MSB_LT5(4),
    _MSB_LT6(5),
    _MSB_LT7(6),
    _MSB_LT8(7)
};


const char *BUILD_TIME = __DATE__ " - " __TIME__;


GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB8(GUINT8 n)
{
    return _MSB_LUT[n];
}

GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB16(GUINT16 n)
{
    unsigned int t( n >> 8 );
    return t ? 8 + _MSB_LUT[t] : _MSB_LUT[n];
}


GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB32(GUINT32 n)
{
    int res;
    unsigned int t1, t2;
    if((t1 = n >> 16))
    {
      res = (t2 = t1 >> 8) ? 24 + _MSB_LUT[t2] :
                           16 + _MSB_LUT[t1];
    }
    else
    {
      res = (t2 = n >> 8) ? 8 + _MSB_LUT[t2] :
                          _MSB_LUT[n];
    }
    return res;
}


GUTIL_COM_EXTERN GUTIL_COM_DECLSPEC int FSB64(GUINT64 n)
{
    int res;
    GUINT64 t1, t2;
    if((t2 = n >> 32))
    {
        if((t1 = t2 >> 16))
        {
            res = (t2 = t1 >> 8) ? 56 +  _MSB_LUT[t2] :
                                   48 + _MSB_LUT[t1];
        }
        else
        {
            res = (t2 = t1 >> 8) ? 40 +  _MSB_LUT[t2] :
                                   32 + _MSB_LUT[t1];
        }
    }
    else if((t1 = n >> 16))
    {
      res = (t2 = t1 >> 8) ? 24 + _MSB_LUT[t2] :
                           16 + _MSB_LUT[t1];
    }
    else
    {
      res = (t2 = n >> 8) ? 8 + _MSB_LUT[t2] :
                          _MSB_LUT[n];
    }
    return res;
}



/** A class to take care of any library initialization code.

    There is a static global instance in the core library, which means
    that the constructor will be called when the library is loaded, and
    its destructor will be called when the library is unloaded.

    This takes care of tasks like seeding the pseudo-random number generator.
*/
class GUTIL_INITIALIZE
{
    static GUTIL_INITIALIZE GUTIL_INITIALIZER;
public:

    inline GUTIL_INITIALIZE()
    {
        // Seed the pseudo RNG
        srand( time(NULL) );
    }

    inline ~GUTIL_INITIALIZE()
    {

    }

};

// Instantiate the initializer class, which causes the
GUTIL_INITIALIZE GUTIL_INITIALIZE::GUTIL_INITIALIZER;


#if !defined(GUTIL_COM_EXPORTS) && !defined(GUTIL_COM_IMPORTS)
END_NAMESPACE_GUTIL;
#endif
