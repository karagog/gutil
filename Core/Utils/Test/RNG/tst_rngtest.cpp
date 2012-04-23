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

    printf("After %i coin tosses, %f percent were Heads", NUM_TRIES, ((double)cnt)/NUM_TRIES);
    QVERIFY(GUtil::ApproximatelyEqual(0.5, ((double)cnt)/NUM_TRIES, 0.001));
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

    printf("After %i generated variables, %f percent were 'Successful', and %f probability was desired", NUM_TRIES, ((double)cnt)/NUM_TRIES, percentage);
    QVERIFY(GUtil::ApproximatelyEqual(percentage, ((double)cnt)/NUM_TRIES, 0.001));
}

void RNGTest::test_UniformInteger()
{
    const int range(10);
    int cnt[range] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(int i = 0; i < NUM_TRIES; ++i){
        GINT32 val = RNG::UniformIntegerBetween(1, 10);
        GASSERT2(0 < val && 11 > val, String::Format("After %i tries I got %i", i, val).ConstData());
        cnt[val - 1] += 1;
    }


    printf("\n\n");
    for(int i = 0; i < range; ++i)
        printf("%i: %f\n", i + 1, static_cast<GFLOAT64>(cnt[i]) / NUM_TRIES);

    for(int i = 0; i < range; ++i)
        QVERIFY2(GUtil::ApproximatelyEqual(static_cast<GFLOAT64>(cnt[i]) / NUM_TRIES,
                                          1.0 / range, 0.001),
                 String::Format("%d", i).ConstData());
}

void RNGTest::test_Normal()
{
    const GFLOAT64 desired_mean( 85.5 );
    const GFLOAT64 desired_stdev( 14 );

    Vector<GFLOAT64> mem(NUM_TRIES);
    GFLOAT64 sum(0);

    for(GUINT32 i = 0; i < NUM_TRIES; i += 2){
        GFLOAT64 val1, val2;
        RNG::Normal2(val1, val2, desired_mean, desired_stdev);
        mem.PushBack(val1);
        mem.PushBack(val2);
        sum += val1 + val2;
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

    printf("The mean (calculated empirically) is %f\n", empirical_mean);

    printf("The standard deviation (calculated empirically) is %f\n", empirical_stdev);

    QVERIFY(GUtil::ApproximatelyEqual(empirical_mean, desired_mean, 0.1));
    QVERIFY(GUtil::ApproximatelyEqual(empirical_stdev, desired_stdev, 0.1));
}

void RNGTest::test_NormalInteger()
{
    const GFLOAT64 desired_mean( 30 );
    const GFLOAT64 desired_stdev( 50 );

    Vector<GINT32> mem(NUM_TRIES);
    GINT32 sum(0);

    for(GUINT32 i = 0; i < NUM_TRIES; i += 2){
        GINT32 val1, val2;
        RNG::NormalInteger2(val1, val2, desired_mean, desired_stdev);
        mem.PushBack(val1);
        mem.PushBack(val2);
        sum += val1 + val2;
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

    printf("The mean (calculated empirically) is %f\n", empirical_mean);

    printf("The standard deviation (calculated empirically) is %f\n", empirical_stdev);

    QVERIFY(GUtil::ApproximatelyEqual(empirical_mean, desired_mean, 0.5));
    QVERIFY(GUtil::ApproximatelyEqual(empirical_stdev, desired_stdev, 0.5));
}

QTEST_APPLESS_MAIN(RNGTest)

#include "tst_rngtest.moc"
