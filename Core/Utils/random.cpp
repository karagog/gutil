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

#include "gutil_random.h"

// We have to include this, for the correct sourcing of the preprocessor
//  macro GUTIL_NO_RNG
#include "gutil_globals.h"


#ifndef GUTIL_NO_RNG

#include "ThirdParty/cryptopp-5.6.1/osrng.h"

/** We allocate a global static RNG from CryptoPP, so we don't
    have to allocate one every time we need random data.
*/
static ::CryptoPP::AutoSeededX917RNG< ::CryptoPP::AES > _rng;

#endif  // GUTIL_NO_CRYPTOPP


NAMESPACE_GUTIL1(Utils);


void RandomData::Fill(GBYTE *buffer, GUINT32 size)
{
#ifndef GUTIL_NO_RNG
    _rng.GenerateBlock(buffer, size);
#else
    GBYTE const *e( buffer + size );
    while(buffer != e)
        *(buffer++) = rand();
#endif
}


END_NAMESPACE_GUTIL1;
