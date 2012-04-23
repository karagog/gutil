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

#include "rng.h"
#include "gutil_smartpointer.h"
#include <math.h>

#ifndef GUTIL_NO_RNG

#include "ThirdParty/cryptopp-5.6.1/osrng.h"

/** We allocate a global static RNG from CryptoPP, so we don't
    have to allocate one every time we need random data.
*/
static ::GUtil::Utils::SmartPointer< ::CryptoPP::AutoSeededX917RNG< ::CryptoPP::AES > > _rng;

#endif  // GUTIL_NO_RNG



NAMESPACE_GUTIL1(Utils);


void RNG::Initialize()
{
#ifndef GUTIL_NO_RNG
    if(_rng.IsNull())
        _rng = new ::CryptoPP::AutoSeededX917RNG< ::CryptoPP::AES >;
#else
    // The rand() function is automatically seeded in the GUtil library initializer,
    //  so there is no setup necessary
#endif
}

void RNG::Uninitialize()
{
#ifndef GUTIL_NO_RNG
    _rng.Clear();
#endif
}

void RNG::Fill(GBYTE *buffer, GUINT32 size)
{
#ifndef GUTIL_NO_RNG
    // In release mode we don't want to check every time if it's initialized
    GASSERT( !_rng.IsNull() );

    _rng->GenerateBlock(buffer, size);
#else
    GBYTE const *e( buffer + size );
    while(buffer != e)
        *(buffer++) = rand();
#endif
}

GFLOAT64 RNG::Uniform()
{
    return (static_cast<GFLOAT64>(RNG::Generate<GUINT32>()) + 1.0) /
           (static_cast<GFLOAT64>(GUINT32_MAX) + 2.0);
}

GINT32 RNG::UniformIntegerBetween(GINT32 lower_bound, GINT32 upper_bound)
{
    return floor( UniformBetween(lower_bound, upper_bound + 1) );
}



GFLOAT64 RNG::Normal()
{
    // I use the Box-Muller method due to the ease of implementation
    //  and the quality of the normally-distributed random data.
    return sqrt(-2.0 * log(Uniform())) * sin( 2.0 * PI * Uniform() );
}

void RNG::Normal2(GFLOAT64 &n1, GFLOAT64 &n2)
{
    GFLOAT64 U(Uniform()), V(Uniform());
    GFLOAT64 C( sqrt(-2.0 * log(U)) );
    GFLOAT64 A( 2.0 * PI * V );
    n1 = C * sin(A);
    n2 = C * cos(A);
}

GINT32 RNG::NormalInteger(const GFLOAT64 &mean, const GFLOAT64 &standard_deviation)
{
   return floor( Normal(mean + 0.5, standard_deviation) );
}

void RNG::NormalInteger2(int &n1, int &n2, const double &mean, const double &standard_deviation)
{
    GFLOAT64 f1, f2;
    Normal2(f1, f2, mean + 0.5, standard_deviation);
    n1 = floor(f1);
    n2 = floor(f2);
}


END_NAMESPACE_GUTIL1;
