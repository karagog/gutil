
#include "ThirdParty/fdlibm-5.2/fdlibm.h"
#include "gutil_rng.h"
#include "gutil_strings.h"
#include <stdio.h>
using namespace GUtil::Utils;
using namespace GUtil::DataObjects;

#define NUM_TRIES 1000000


void test_cointoss();
void test_percent_success();
void test_uniform_integer();
void test_normal_integer();

int main(int, char**)
{
    RNG::Initialize();

    //test_cointoss();
    //test_percent_success();
    test_uniform_integer();
    //test_normal_integer();

    return 0;
}


void test_cointoss()
{
    int cnt(0);

    for(int i = 0; i < NUM_TRIES; ++i)
    {
        if(RNG::CoinToss())
            cnt++;
    }

    printf("After %i coin tosses, %f percent were Heads", NUM_TRIES, ((double)cnt)/NUM_TRIES);
}

void test_percent_success()
{
    int cnt(0);
    double percentage(0.2);

    for(int i = 0; i < NUM_TRIES; ++i)
    {
        if(RNG::Succeed(percentage))
            cnt++;
    }

    printf("After %i generated variables, %f percent were 'Successful', and %f probability was desired", NUM_TRIES, ((double)cnt)/NUM_TRIES, percentage);
}


void test_uniform_integer()
{
    int cnt[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(int i = 0; i < NUM_TRIES; ++i){
        GINT32 val = RNG::UniformIntegerBetween(1, 10);
        GASSERT2(0 < val && 11 > val, String::Format("After %i tries I got %i", i, val).ConstData());
        cnt[val - 1] += 1;
    }


    for(int i = 0; i < 10; ++i)
        printf("%i: %f\n", i + 1, static_cast<GFLOAT64>(cnt[i]) / NUM_TRIES);
}


/*
void test_normal()
{
    printf("The number of values within 1 standard deviation is %i, or %f percent\n",
                one_stdev, static_cast<GFLOAT64>(one_stdev) / NUM_TRIES);

    printf("The number of values within 2 standard deviations is %i, or %f percent\n",
                two_stdev, static_cast<GFLOAT64>(two_stdev) / NUM_TRIES);

    printf("The number of values within 3 standard deviations is %i, or %f percent\n",
                three_stdev, static_cast<GFLOAT64>(three_stdev) / NUM_TRIES);
}
*/


void test_normal_integer()
{
    int cnt[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    GUINT32 sum(0);
    GUINT32 throw_aways(0);

    for(int i = 0; i < NUM_TRIES; ++i){
        GINT32 val = RNG::NormalInteger(5, 1);
        if(val < 11 && val >= 0){
            cnt[val] += 1;
            sum += val;
        }
        else
            throw_aways++;
    }


    for(int i = 0; i < 11; ++i)
        printf("%i: %f\n", i, static_cast<GFLOAT64>(cnt[i]) / (NUM_TRIES - throw_aways));

    GFLOAT64 empirical_mean( static_cast<GFLOAT64>(sum) / (NUM_TRIES - throw_aways) );
    GFLOAT64 empirical_stdev( 0 );

    // Calculate the standard deviation for the sample
    for(int i = 0; i < 11; ++i){
        GFLOAT64 diff_to_mean( i - empirical_mean );
        diff_to_mean *= diff_to_mean;
        empirical_stdev += cnt[i] * diff_to_mean;
    }
    empirical_stdev = empirical_stdev / (NUM_TRIES - throw_aways - 1);
    empirical_stdev = sqrt(empirical_stdev);

    printf("\n");

    printf("The mean (calculated empirically) is %f\n", empirical_mean);

    printf("The standard deviation (calculated empirically) is %f\n", empirical_stdev);

    printf("Had to throw away %i samples out of %i because they were outside the desired range\n", throw_aways, NUM_TRIES);
}

