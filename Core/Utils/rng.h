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

#ifndef GUTIL_RNG_H
#define GUTIL_RNG_H

#include "gutil_globals.h"

NAMESPACE_GUTIL1(Utils);


/** A convenience class that gives you useful Random Number Generator functions.

    By default, the library uses CryptoPP's RNG implementation, which
    is extremely good.  However, if you want to disable this and simply
    use C's rand() function as a random data source, you can define
    GUTIL_NO_CRYPTOPP. Your random data won't be nearly as good,
    but at least you won't depend on the cryptopp library.

    \sa Initialize()
*/
class RNG
{
    GUTIL_STATIC_CLASS(RNG);
public:

    /** Initializes the RNG, including setting up necessary data objects
        in memory and seeding the RNG.

        You must call this before using any RNG-dependent functions!

        \sa Uninitialize()
    */
    static void Initialize();

    /** Uninitializes the RNG.

        You do not have to call this, but it is there in case
        you want to free the resources used by the RNG.  This memory will automatically
        be reclaimed when the GUtil library is unloaded at the end of the program's execution.

        After calling this, you will have to call Initialize() again before you can use
        the RNG.

        \sa Initialize()
    */
    static void Uninitialize();

    /** Returns a uniformly distributed number between 0 and 1, EXCLUSIVE.
        The range of the return value is (0, 1).

        \note The return value will never be exactly 0 or 1, which simplifies
        some algorithms without sacrificing any quality.  For example,
        the Box-Muller method for generating normal distributions requires
        a log() operation,  which fails in the case of 0 input.  This Uniform convention is also
        arguably compliant with probabilistic theory, because the probability
        of getting a 0 or 1 exactly is 0%.

        \note The resolution of the random return value is about 1/2^32
    */
    static GFLOAT64 Uniform();

    /** Returns a uniformly distributed random number between the two bounds, EXCLUSIVE.
        The range of the return value is (lower_bound, upper_bound).
    */
    static inline GFLOAT64 UniformBetween(const GFLOAT64 &lower_bound, const GFLOAT64 &upper_bound){
        GASSERT(lower_bound <= upper_bound);
        return (Uniform() * (upper_bound - lower_bound)) + lower_bound;
    }

    /** Returns a uniformly distributed integer value between the given
        lower and upper bounds, INCLUSIVE.  Each integer in the return range
        is equally likely to be returned.

        The range of the return value is [lower_bound, upper_bound].
    */
    static GINT32 UniformIntegerBetween(GINT32 lower_bound, GINT32 upper_bound);

    /** Returns a (approximate) standard normal random number
        with a mean of 0 and standard deviation of 1.

        \note The resolution of the random return value is about 1/2^32
    */
    static GFLOAT64 Normal();

    /** Returns a normally distributed random number with the given mean
        and standard deviation.

        \param mean The desired mean of the normal variable.
        \param standard_deviation The desired standard deviation of the normal variable.

        \note The resolution of the random return value is about standard_deviation/(2^32 - 1)
    */
    static inline GFLOAT64 Normal(const GFLOAT64 &mean, const GFLOAT64 &standard_deviation){
        return (Normal() * standard_deviation) + mean;
    }

    /** A special version of Normal that produces 2 values instead of 1.

        Use this version if you will be generating lots of random values,
        because the regular implementation practically generates two values anyways.
    */
    static void Normal2(GFLOAT64 &n1, GFLOAT64 &n2);

    /** A special version of Normal that produces 2 values instead of 1,
        with the given mean and standard deviation.

        Use this version if you will be generating lots of random values,
        because the regular implementation practically generates two values anyways.
    */
    static inline void Normal2(GFLOAT64 &n1, GFLOAT64 &n2,
                               const GFLOAT64 &mean, const GFLOAT64 &standard_deviation){
       Normal2(n1, n2);
       n1 = (n1 * standard_deviation) + mean;
       n2 = (n2 * standard_deviation) + mean;
    }

    /** Returns a normally distributed integer with the given mean and standard deviation. */
    static GINT32 NormalInteger(const GFLOAT64 &mean, const GFLOAT64 &standard_deviation);

    /** Returns two normally distributed integers with the given mean and standard deviation.

        Use this version if you will be generating lots of random values,
        because the regular implementation practically generates two values anyways.
    */
    static void NormalInteger2(GINT32 &n1, GINT32 &n2, const GFLOAT64 &mean, const GFLOAT64 &standard_deviation);



    /** Returns a random data object, whose type is specified by the
        template parameter.

        \tparam T The type of the return value.
    */
    template<class T>
    static inline T Generate(){
        T ret;
        Fill(reinterpret_cast<GBYTE *>(&ret), sizeof(T));
        return ret;
    }

    /** Fills the buffer with random data. */
    static void Fill(GBYTE *buffer, GUINT32 size);



    /** Returns true with the given probability.

        \param probability_of_success The probability that this function
        will return true, given as a percentage between 0 and 1.

        \note If the given probability of success is negative, this function
        will always return false.  Conversely, if the probability is greater
        than or equal to 1, this function will always return true.
    */
    static inline bool Succeed(const GFLOAT64 &probability_of_success){
        return Uniform() < probability_of_success;
    }

    /** Optimized version of Succeed() that returns True with a 50% probability. */
    static inline bool CoinToss(){
        return Generate<GINT8>() < 0;
    }

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_RNG_H
