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

#include "globals.h"
#include <gutil/macros.h>
#include <gutil/rng.h>
#include <cstdlib>
#include <time.h>


#ifdef GUTIL_DEBUG
    #include <stdio.h>
    #define ASSERTION_TEXT_TEMPLATE  "Assertion failed: (%s) on line %d of %s"

    void __gutil_debug(const char *s)
    {
        printf("%s\n", s);
        fflush(stdout);
    }

    void __gutil_assertion_failed(const char *assertion_text, const char *filename, int line_number)
    {
        printf(ASSERTION_TEXT_TEMPLATE "\n", assertion_text, line_number, filename);
        abort();
    }

    void __gutil_assertion_failed(const char *assertion_text, const char *filename, int line_number, const char *message)
    {
        printf(ASSERTION_TEXT_TEMPLATE ": %s\n", assertion_text, line_number, filename, message);
        abort();
    }
#endif


#if !defined(GUTIL_COM_EXPORTS) && !defined(GUTIL_COM_IMPORTS)
NAMESPACE_GUTIL;
#endif


/** A lookup table for quickly determining the first set bit of an integer */
static GBYTE const _MSB_LUT[] =
{
    #define _MSB_LUT2(n) n, n
    #define _MSB_LUT3(n) _MSB_LUT2(n), _MSB_LUT2(n)
    #define _MSB_LUT4(n) _MSB_LUT3(n), _MSB_LUT3(n)
    #define _MSB_LUT5(n) _MSB_LUT4(n), _MSB_LUT4(n)
    #define _MSB_LUT6(n) _MSB_LUT5(n), _MSB_LUT5(n)
    #define _MSB_LUT7(n) _MSB_LUT6(n), _MSB_LUT6(n)
    #define _MSB_LUT8(n) _MSB_LUT7(n), _MSB_LUT7(n)

    0xFF, // Returns error if no bits are set
    0,
    _MSB_LUT2(1),
    _MSB_LUT3(2),
    _MSB_LUT4(3),
    _MSB_LUT5(4),
    _MSB_LUT6(5),
    _MSB_LUT7(6),
    _MSB_LUT8(7)
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

    This takes care of tasks like seeding the pseudo-random number generator,
    for example.

    \note This is a private class, designed to be used only by the library
    developers.  You should not have to modify it in any way for your daily
    programming needs.
*/
class LOADER
{
public:

    /** Any library initialization code gets executed when the library is loaded. */
    LOADER(){
        GDEBUG("Initializing GUtil Core Library...");
    }

    /** Any library cleanup code gets executed when the library is unloaded. */
    ~LOADER(){
        GDEBUG("Uninitializing GUtil Core Library...");
    }

};

// Instantiate the initializer class, which takes care of library init/takedown
static LOADER __ldr;


#if !defined(GUTIL_COM_EXPORTS) && !defined(GUTIL_COM_IMPORTS)
END_NAMESPACE_GUTIL;
#endif
