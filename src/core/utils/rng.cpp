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
USING_NAMESPACE_GUTIL;

#ifndef GUTIL_NO_CRYPTOPP

#include "cryptopp/osrng.h"

/** We allocate a global static RNG from CryptoPP, so we don't
    have to allocate one every time we need random data.
*/
static GUtil::SmartPointer< ::CryptoPP::AutoSeededX917RNG< ::CryptoPP::AES > > __rng;

#else

#include <cstdlib>
#include <ctime>

#endif  // GUTIL_NO_CRYPTOPP



/** Scales down an integer in the range [0, GUINT32_MAX] to a
    floating point number in the range (0, 1).
*/
static GFLOAT64 __rng_int_to_unit_float(GUINT32 i);

/** Generates two integers with one call to Generate(). */
static Pair<GUINT32> __rng_generate_two_numbers();



NAMESPACE_GUTIL;


void RNG::Initialize()
{
#ifndef GUTIL_NO_CRYPTOPP
    if(__rng.IsNull())
        __rng = new ::CryptoPP::AutoSeededX917RNG< ::CryptoPP::AES >;
#else
    srand(time(NULL));
#endif
}

void RNG::Uninitialize()
{
#ifndef GUTIL_NO_CRYPTOPP
    if(!__rng.IsNull())
        __rng.Clear();
#endif
}

void RNG::Fill(GBYTE *buffer, GUINT32 size)
{
#ifndef GUTIL_NO_CRYPTOPP
    if(__rng.IsNull())
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "The RNG has not been initialized");

    __rng->GenerateBlock(buffer, size);
#else
    GBYTE const *e( buffer + size );
    while(buffer != e)
        *(buffer++) = rand();
#endif
}

GFLOAT64 RNG::U(GFLOAT64 lower_bound, GFLOAT64 upper_bound)
{
    GASSERT(lower_bound <= upper_bound);

    return lower_bound + __rng_int_to_unit_float(Generate<GUINT32>()) * (upper_bound - lower_bound);
}

GINT32 RNG::U_Discrete(GINT32 lower_bound, GINT32 upper_bound)
{
    return floor( U(lower_bound, upper_bound + 1) );
}



GFLOAT64 RNG::N(GFLOAT64 mean, GFLOAT64 standard_deviation)
{
    // I use the Box-Muller method due to the ease of implementation
    //  and the quality of the normally-distributed random data, but
    //  not for its efficiency.

    // Generate two independent U(0, 1) values (optimized)
    const Pair<GUINT32> p( __rng_generate_two_numbers() );
    const GFLOAT64 u( __rng_int_to_unit_float(p.First) );
    const GFLOAT64 v( __rng_int_to_unit_float(p.Second) );

    return mean + standard_deviation * sqrt(-2.0 * log(u) ) * sin( 2.0 * PI * v );
}

Pair<GFLOAT64> RNG::N2(GFLOAT64 mean, GFLOAT64 standard_deviation)
{
    // Generate two independent U(0, 1) values (optimized)
    const Pair<GUINT32> p( __rng_generate_two_numbers() );
    const GFLOAT64 u( __rng_int_to_unit_float(p.First) );
    const GFLOAT64 v( __rng_int_to_unit_float(p.Second) );

    // Using those two random values between 0 and 1, we can
    //  generate two normally distributed variates.
    const GFLOAT64 C( sqrt(-2.0 * log(u)) );
    const GFLOAT64 A( 2.0 * PI * v );

    //printf("%8X %8X\n", p.First, p.Second);

    return Pair<GFLOAT64>(mean + standard_deviation * C * sin(A), mean + standard_deviation * C * cos(A));
}

GINT32 RNG::N_Discrete(GFLOAT64 mean, GFLOAT64 standard_deviation)
{
   return floor( N(mean + 0.5, standard_deviation) );
}

Pair<GINT32> RNG::N_Discrete2(GFLOAT64 mean, GFLOAT64 standard_deviation)
{
    Pair<GINT32> ret;
    Pair<GFLOAT64> f( N2(mean + 0.5, standard_deviation) );
    ret.First = floor(f.First);
    ret.Second = floor(f.Second);
    return ret;
}

GINT32 RNG::Poisson(GFLOAT32 expected_value)
{
    // This algorithm was taken from Wikipedia.  I honestly don't know why it works, but you
    //  can validate it by studying the output of the function.
    GINT32 ret( -1 );

    if(0.0 < expected_value)
    {
        GFLOAT64 p( 1.0 );
        const GFLOAT64 L( exp( -expected_value ) );

        while(p > L)
        {
            p *= U(0, 1);
            ret += 1;
        }
    }

    return ret;
}

GUINT64 RNG::Geometric(GFLOAT32 e)
{
    GUINT64 ret;
    if(1.0 < e)
    {
        // Valid input, and no special case to worry about, so generate value
        ret = (GUINT64)( log(U(0,1)) / log(1.0 - 1.0 / e) ) + 1;
    }
    else if(1.0 > e)
    {
        // Invalid input, return 0
        ret = 0;
    }
    else
    {
        // Special case where expected value == 1, because log(0) is undefined
        // The probability in this case is 100%, so just return 1
        ret = 1;
    }
    return ret;
}

GFLOAT64 RNG::Exponential(GFLOAT64 lambda)
{
    GFLOAT64 ret(-1.0);
    if(0.0 < lambda)
        ret = -log( U(0, 1) ) / lambda;
    return ret;
}


END_NAMESPACE_GUTIL;



GFLOAT64 __rng_int_to_unit_float(GUINT32 i)
{
    // NOTE: The addition of 1 in the numerator and 2 in the denominator
    //  are intended so the result never equals the lower or upper bounds,
    //  but the difference is so small that it doesn't measurably affect
    //  the probability distribution
    return (1.0 + i) / (2.0 + GUINT32_MAX);
}

Pair<GUINT32> __rng_generate_two_numbers()
{
    GUINT64 L( GUtil::RNG::Generate<GUINT64>() );
    return Pair<GUINT32>(L >> 32, L);
}
