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
#include "gutil_pair.h"

NAMESPACE_GUTIL;


/** A convenience class that gives you useful Random Number Generator functions.

    By default, the library uses CryptoPP's RNG implementation, which
    is extremely good.  However, if you want to disable this and simply
    use C's rand() function as a random data source, you can define
    GUTIL_NO_CRYPTOPP. Your random data won't be nearly as good,
    but at least you won't depend on the cryptopp library.

    \note The RNG is automatically initialized when the GUtil core library is loaded,
    so simply by linking with this library you have already initialized the RNG.  Therefore
    this just works "out of the box".

    \sa globals.cpp
*/
class RNG
{
    GUTIL_STATIC_CLASS(RNG);
public:

    /** Initializes the RNG, including setting up necessary data objects
        in memory and seeding the RNG.

        You must call this before using any RNG-dependent functions!

        \note This does nothing if the RNG was already initialized, so it
        doesn't hurt to call it.

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

    /** Returns a uniformly distributed random number between the two bounds, EXCLUSIVE.

        \return A floating point number in the range (lower_bound, upper_bound)

        \note The return value will never be exactly the lower or upper bound, which simplifies
        some algorithms without sacrificing any quality.  For example,
        the Box-Muller method for generating normal distributions requires
        a log() operation,  which fails in the case of 0 input.  This Uniform convention is also
        arguably compliant with probabilistic theory, because the probability
        of getting either bound in a continuous distribution is 0%.

        \note The resolution of the random return value is about (upper_bound - lower_bound)/2^32
    */
    static GFLOAT64 U(GFLOAT64 lower_bound = 0.0, GFLOAT64 upper_bound = 1.0);

    /** Returns a uniformly distributed integer value between the given
        lower and upper bounds, INCLUSIVE.  Each integer in the return range
        is equally likely to be returned.

        \return An integer value in the range [lower_bound, upper_bound].
    */
    static GINT32 U_Discrete(GINT32 lower_bound, GINT32 upper_bound);

    /** Returns a normally distributed random number with the given mean
        and standard deviation.

        \param mean The desired mean of the normal variable.  The default is 0.
        \param standard_deviation The desired standard deviation of the normal variable.
        The default is 1.

        \note The resolution of the random return value is about standard_deviation/2^32
    */
    static GFLOAT64 N(GFLOAT64 mean = 0.0, GFLOAT64 standard_deviation = 1.0);

    /** A special version of Normal that produces 2 values instead of 1.

        Use this version if you will be generating lots of random values,
        because the regular implementation practically generates two values anyways.
    */
    static GUtil::Pair<GFLOAT64> N2(GFLOAT64 mean = 0.0,
                                                   GFLOAT64 standard_deviation = 1.0);

    /** Returns a normally distributed integer with the given mean and standard deviation. */
    static GINT32 N_Discrete(GFLOAT64 mean = 0.0, GFLOAT64 standard_deviation = 1.0);

    /** Returns two normally distributed integers with the given mean and standard deviation.

        Use this version if you will be generating lots of random values,
        because the regular implementation practically generates two values anyways.
    */
    static GUtil::Pair<GINT32> N_Discrete2(GFLOAT64 mean = 0.0, GFLOAT64 standard_deviation = 1.0);


    /** Generates a Poisson-distributed random variable.

        \param expected_value The mean value of the random variable.
        This value must be strictly > 0, otherwise the function returns -1.
        This is the only parameter necessary to generate a Poisson
        distribution from a U(0, 1) distribution.  It describes the
        rate at which the event  occurs.

        \return A discrete random variable with a Poisson distribution, and
        a mean value of expected_value.

        \note The complexity of the implementation is O(expected_value),
        so be careful when calling with large numbers
    */
    static GINT32 Poisson( GFLOAT32 expected_value );


    /** Generates a discrete, Geometrically-distributed random variable
        with the given expected value.

        The definition of a Geometric random variable used here is:

        P(X = n) = (1 − p)^(n−1) * p

        ...which means that given a random trial that is successful with a certain probability,
        and is independent of all other trials in the sequence,
        the geometric random variable is the number of trials until your first success.

        \param expected_value The mean value of the generated
        random variable.   This value must be greater-than or equal
        to 0, otherwise the function returns -1.  For example, a coin-toss would
        have an expected value of 2, because with a probability of success of 50%
        it is expected it will take 2 tosses to get a win.

        \return A positive, discrete geometrically-distributed random variable.
        In the case of an invalid input (expected_value less than 1) then 0 is returned.
        The range of valid return values are [1, GUINT64_MAX], but if you have
        a really large expected value (super low probability of success) then you should
        know that there is no overflow in the return value, and if the value generated was
        larger than the max then it will be capped at the max value of the return type.

        \note This implementation has complexity O(1).  I borrowed the
        mathematical concept from a paper I found online.  Open source is
        great, isn't it??  You scratch my back, I'll scratch yours.
        Without proving it (I'll leave that as an exercise for the reader),
        here is the algorithm:
        Int( log(U)/log(1-p) ) + 1
        Where Int(Y) is the integer part of Y.
    */
    static GUINT64 Geometric( GFLOAT32 expected_value );


    /** Generates an Exponentially distributed random variable
          with the rate parameter lambda.

          The probability density function is P(x; x > 0) = lambda * exp(-lambda*x)

          This probability distribution can be used to simulate potential
          event occurrences which happen randomly over time, but
          at a known average rate.  For example, if you know that
          100 meteors strike the earth every year on average, then
          you can simulate the amount of time that passes between
          meteor strikes by generating variates from this distribution
          with 100 as the rate parameter (The unit of the return value
          would then be a fraction of 1 year; the unit being defined
          by the unit of the rate parameter).

          \param lambda Describes the rate of the exponential process.
          This parameter must be strictly greater than 0.

          \return A positive number with an exponential distribution
          and mean value 1/lambda.  The variance is 1/(lambda^2).
          If the input parameter is invalid (less than or equal to 0),
          then the return value is -1.
    */
    static GFLOAT64 Exponential( GFLOAT64 lambda );



    /** Returns a random data object, whose type is specified by the
        template parameter.

        \tparam T The type of the return value.  This must be a
        Plain-Old-Data (POD) type, like an Int or Float, because the
        object's memory will be randomly generated.
    */
    template<class T> static T Generate(){
        T ret;
        Fill(reinterpret_cast<GBYTE *>(&ret), sizeof(T));
        return ret;
    }

    /** Fills the buffer with random data. */
    static void Fill(GBYTE *buffer, GUINT32 size);



    /** A discrete random variable which is 1 with the given probability
        and 0 otherwise.

        \param probability_of_success The probability that this function
        will return 1, given as a decimal value between 0 and 1.
        The value given may be outside the range [0, 1], but if it is
        then the return value will always be either a 0 or a 1, depending
        on which bound is being violated.

        \return 1 with the given probability  and 0 otherwise.
    */
    static int Succeed(GFLOAT64 probability_of_success){
        return U(0, 1) < probability_of_success ? 1 : 0;
    }

    /** Optimized version of Succeed() that returns 1 with a 50% probability.

        In the case of a coin toss, you do not need a random value
        with such a great resolution as the normal Succeed() function;
        just generating one random byte is sufficient.

        \return 1 with 50% probability, otherwise 0.
    */
    static int CoinToss(){
        return Generate<GINT8>() < 0 ? 1 : 0;
    }

};


/** Create a static instance of this in any library making use of the RNG
 *  to make sure that it is properly initialized and ready for use.
 *
 *  \example static RNG_Initializer __rng_init;
*/
class RNG_Initializer
{
public:
    inline RNG_Initializer(){ RNG::Initialize(); }
};


END_NAMESPACE_GUTIL;

#endif // GUTIL_RNG_H
