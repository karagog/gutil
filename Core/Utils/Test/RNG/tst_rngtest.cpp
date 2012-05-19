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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include "gutil_rng.h"
#include "gutil_strings.h"
#include "gutil_globals.h"
USING_NAMESPACE_GUTIL1(Utils);
USING_NAMESPACE_GUTIL1(DataObjects);

#define NUM_TRIES 1000000U

// Control whether to dump debug output to the command line
bool OUTPUT_ENABLED = false;


class RNGTest : public QObject
{
    Q_OBJECT

public:
    RNGTest();

private Q_SLOTS:
    void test_CoinToss();

    void test_PercentSuccess();

    void test_UniformInteger();

    void test_Normal();
    void test_NormalInteger();

    void test_Poisson();

    void test_Geometric();
    void test_Exponential();
};

RNGTest::RNGTest()
{
    RNG::Initialize();
}

void RNGTest::test_CoinToss()
{
    int cnt(0);

    for(int i = 0; i < NUM_TRIES; ++i)
    {
        if(RNG::CoinToss())
            cnt++;
    }

    if(OUTPUT_ENABLED)
        printf("After %i coin tosses, %2.4f%% percent were Heads", NUM_TRIES, ((double)cnt)/NUM_TRIES * 100);
        
    QVERIFY(0 == GUtil::FuzzyCompare(0.5, ((double)cnt)/NUM_TRIES, 0.01));
}

void RNGTest::test_PercentSuccess()
{
    int cnt(0);
    double percentage(0.2);

    for(int i = 0; i < NUM_TRIES; ++i)
    {
        if(RNG::Succeed(percentage))
            cnt++;
    }

    if(OUTPUT_ENABLED)
        printf("After %i generated variables, %2.4f percent were 'Successful', and %2.4f probability was desired", NUM_TRIES, ((double)cnt)/NUM_TRIES*100, percentage*100);
        
    QVERIFY(0 == GUtil::FuzzyCompare(percentage, ((double)cnt)/NUM_TRIES, 0.001));
}

void RNGTest::test_UniformInteger()
{
    const int range(10);
    int cnt[range] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(int i = 0; i < NUM_TRIES; ++i){
        GINT32 val = RNG::U_Discrete(1, 10);
        GASSERT2(0 < val && 11 > val, String::Format("After %i tries I got %i", i, val).ConstData());
        cnt[val - 1] += 1;
    }


    if(OUTPUT_ENABLED){
        printf("\n\n");
        for(int i = 0; i < range; ++i)
            printf("%i: %2.4f%%\n", i + 1, static_cast<GFLOAT64>(cnt[i]) / NUM_TRIES * 100);
    }

    for(int i = 0; i < range; ++i)
        QVERIFY2(0 == GUtil::FuzzyCompare(static_cast<GFLOAT64>(cnt[i]) / NUM_TRIES,
                                          1.0 / range, 0.01),
                 String::Format("%d", i).ConstData());
}

void RNGTest::test_Normal()
{
    const GFLOAT64 desired_mean( 85.5 );
    const GFLOAT64 desired_stdev( 14 );

    Vector<GFLOAT64> mem(NUM_TRIES);
    GFLOAT64 sum(0);

    for(GUINT32 i = 0; i < NUM_TRIES; i += 2){
        Pair<GFLOAT64> val( RNG::N2(desired_mean, desired_stdev) );
        mem.PushBack(val.First);
        mem.PushBack(val.Second);
        sum += val.First + val.Second;
    }

    GFLOAT64 empirical_mean( sum / NUM_TRIES );
    GFLOAT64 empirical_stdev( 0 );

    // Calculate the standard deviation for the sample
    G_FOREACH_CONST(GFLOAT64 f, mem)
    {
        GFLOAT64 diff_to_mean( f - empirical_mean );
        empirical_stdev += (diff_to_mean * diff_to_mean);
    }
    empirical_stdev = empirical_stdev / (NUM_TRIES - 1);
    empirical_stdev = sqrt(empirical_stdev);

    if(OUTPUT_ENABLED){
        printf("The mean (calculated empirically) is %f\n", empirical_mean);
        printf("The standard deviation (calculated empirically) is %f\n", empirical_stdev);
    }

    QVERIFY(0 == GUtil::FuzzyCompare(empirical_mean, desired_mean, 0.1));
    QVERIFY(0 == GUtil::FuzzyCompare(empirical_stdev, desired_stdev, 0.1));
}

void RNGTest::test_NormalInteger()
{
    const GFLOAT64 desired_mean( 30 );
    const GFLOAT64 desired_stdev( 50 );

    Vector<GINT32> mem(NUM_TRIES);
    GINT32 sum(0);

    for(GUINT32 i = 0; i < NUM_TRIES; i += 2){
        Pair<GINT32> val( RNG::N_Discrete2(desired_mean, desired_stdev) );
        mem.PushBack(val.First);
        mem.PushBack(val.Second);
        sum += val.First + val.Second;
    }

    GFLOAT64 empirical_mean( static_cast<GFLOAT64>(sum) / NUM_TRIES );
    GFLOAT64 empirical_stdev( 0 );

    // Calculate the standard deviation for the sample
    G_FOREACH_CONST(GINT32 f, mem)
    {
        GFLOAT64 diff_to_mean( empirical_mean - f );
        empirical_stdev += (diff_to_mean * diff_to_mean);
    }
    empirical_stdev = empirical_stdev / (NUM_TRIES - 1);
    empirical_stdev = sqrt(empirical_stdev);

    if(OUTPUT_ENABLED){
        printf("The mean (calculated empirically) is %f\n", empirical_mean);
        printf("The standard deviation (calculated empirically) is %f\n", empirical_stdev);
    }

    QVERIFY(0 == GUtil::FuzzyCompare(empirical_mean, desired_mean, 0.5));
    QVERIFY(0 == GUtil::FuzzyCompare(empirical_stdev, desired_stdev, 0.5));
}

void RNGTest::test_Poisson()
{
    GINT32 sum(0);
    GINT32 num_tries(10000);
    GFLOAT64 empirical_mean(0);
    GFLOAT64 desired_mean( 40 );

    for(int i = 0; i < num_tries; ++i)
    {
        GINT32 p = RNG::Poisson(desired_mean);
        sum += p;
        //printf("Poisson: %d\n", p);
    }

    empirical_mean = static_cast<GFLOAT64>(sum) / num_tries;
    
    if(OUTPUT_ENABLED)
        printf("The average of the Poisson variables is %f", empirical_mean);

    QVERIFY(0 == GUtil::FuzzyCompare(empirical_mean, desired_mean, 1.0));
}

void RNGTest::test_Geometric()
{
    const int num_tries( NUM_TRIES );
    GFLOAT64 mean(0.0);
    const GFLOAT64 expected_value(4);
    const GFLOAT64 trial_probability( 1.0 / (expected_value + 1) );
    GINT32 max(-1);
    GINT32 zero_count( 0 );
    GFLOAT64 zero_percent;

    for(int i = 0; i < num_tries; ++i)
    {
        GINT32 T = RNG::Geometric(expected_value);
        mean += T;
        if(max < T)
            max = T;

        if( T == 0 )
            zero_count++;

        //printf("Geometric: %d\n", T);
    }

    mean = mean / num_tries;
    zero_percent = ((GFLOAT64)zero_count) / num_tries;

    if(OUTPUT_ENABLED){
        printf("The average of the Geometric variates is: %f \n", mean);
        printf("The maximum variate produced was: %d \n", max);
        printf("%2.4f%% of the variates produced were 0\n", zero_percent * 100);
    }

    QVERIFY(0 == GUtil::FuzzyCompare(expected_value, mean, 0.1));
    QVERIFY(0 == GUtil::FuzzyCompare(zero_percent, trial_probability, 0.1));
}

void RNGTest::test_Exponential()
{
    const int num_tries( NUM_TRIES );
    GFLOAT64 mean(0.0);
    const GFLOAT64 rate(2);
    GFLOAT64 max(-1);

    for(int i = 0; i < num_tries; ++i)
    {
        GFLOAT64 T = RNG::Exponential(rate);
        mean += T;
        if(max < T)
            max = T;
        //printf("Exponential: %d\n", T);
    }

    mean = mean / num_tries;

    if(OUTPUT_ENABLED){
        printf("The average of the Exponential variates is: %f \n", mean);
        printf("The maximum variate produced was: %f \n", max);
    }

    QVERIFY(0 == GUtil::FuzzyCompare(1.0 / rate, mean, 0.1));
}



QTEST_APPLESS_MAIN(RNGTest)

#include "tst_rngtest.moc"
