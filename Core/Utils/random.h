/*Copyright 2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_RANDOM_H
#define GUTIL_RANDOM_H

#include "gutil_macros.h"

NAMESPACE_GUTIL1(Utils);


/** Generates random data with various helper functions.

    This static class contains helper functions that make it quick and easy
    to get quality random data into your program.

    By default, the library uses CryptoPP's RNG implementation, which
    is extremely good.  However, if you want to disable this and simply
    use C's rand() function as a random data source, you can define
    GUTIL_NO_CRYPTOPP. Your random data won't be nearly as good,
    but at least you won't depend on the cryptopp library.

    \note You do not need to seed the RNG, no matter which implementation
    you use.  The GUtil library automatically calls srand() with the system
    time whenever the library is loaded, and CryptoPP's RNG is auto-seeded.
*/
class RandomData
{
public:

    /** Fills the buffer with random data. */
    static void Fill(GBYTE *buffer, GUINT32 size);

    /** Returns a random data object, whose type is specified by the
        template parameter.

        \tparam INT_TYPE The type of the return value.
    */
    template<class INT_TYPE>
    static inline INT_TYPE Generate(){
        INT_TYPE ret;
        Fill((GBYTE *)&ret, sizeof(INT_TYPE));
        return ret;
    }

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_RANDOM_H
