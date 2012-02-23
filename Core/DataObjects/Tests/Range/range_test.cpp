/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <QTest>
#include "Core/DataObjects/range.h"
#include "Core/DataObjects/gstring.h"
#include "Core/DataAccess/console.h"
USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL1(DataAccess);



#define INFINITY 999999


class RangeTest :
        public QObject
{
    Q_OBJECT

private Q_SLOTS:

    // Test the fundamental Range functions before testing the more complex functions
    void test_range_contains();
    void test_range_intersects();
    void test_range_subset();
    void test_range_complement();

    // Then test the region functions, after the range tests execute
    void test_unions();
    void test_region_complement();

    // If any of these tests fail, it is most likely due to problems in the prior
    //  tests.  If the above tests are passing and below they're failing, then
    //  it suggests your Complement/Union tests are not thorough enough
    void test_intersect();
    void test_difference();
    void test_symmetric_difference();


private:

    /** Dumps the ranges of a region to the console in a human-readable format */
    inline static void print_ranges(const Region<int> &reg){
        int cnt(1);
        if(reg.IsUniverse())
            Console::Write("\nUniverse Set\n");
        else if(reg.IsEmpty())
            Console::Write("\nEmpty Set\n");
        else{
            G_FOREACH_CONST(const Range<int> &r, reg.Ranges()){
                Console::WriteLine();
                Console::Write(String::Format("Range %d:", cnt));
                if(r.HasLowerBound())
                    Console::Write(String::Format("    %sLB: %2d",
                                                  r.LowerBound().ValueIncludedInBound() ? "+" : " ",
                                                  r.LowerBound().Value()));
                if(r.HasUpperBound())
                    Console::Write(String::Format("    %sUB: %2d",
                                                  r.UpperBound().ValueIncludedInBound() ? "+" : " ",
                                                  r.UpperBound().Value()));
                ++cnt;
            }
            Console::WriteLine();
        }
        Console::WriteLine();
    }

};

void RangeTest::test_range_contains()
{
    Range<int> rng;
    QVERIFY(rng.IsEmpty());
    QVERIFY(!rng.IsUniverse());
    QVERIFY(rng.IsBounded());
    QVERIFY(!rng.IsUnbounded());

    rng = Range<int>::Universe();
    QVERIFY(!rng.IsEmpty());
    QVERIFY(rng.IsUniverse());
    QVERIFY(!rng.IsBounded());
    QVERIFY(rng.IsUnbounded());

    // Test the contains functionality

    // Test an inclusive lower bound
    rng = Range<int>::CreateLowerBound(1, true);
    for(int i = -10; i <= 0; i++)
        QVERIFY2(!rng.Contains(i), String::FromInt(i));
    for(int i = 1; i <= 10; i++)
        QVERIFY(rng.Contains(i));
    QVERIFY(rng.Contains(INFINITY));
    QVERIFY(!rng.Contains(-INFINITY));

    // Test an exclusive lower bound
    rng = Range<int>::CreateLowerBound(1, false);
    for(int i = -10; i <= 1; i++)
        QVERIFY2(!rng.Contains(i), String::FromInt(i));
    for(int i = 2; i <= 10; i++)
        QVERIFY(rng.Contains(i));
    QVERIFY(rng.Contains(INFINITY));
    QVERIFY(!rng.Contains(-INFINITY));

    // Test an inclusive upper bound
    rng = Range<int>::CreateUpperBound(1, true);
    for(int i = -10; i <= 1; i++)
        QVERIFY2(rng.Contains(i), String::FromInt(i));
    for(int i = 2; i <= 10; i++)
        QVERIFY(!rng.Contains(i));
    QVERIFY(rng.Contains(-INFINITY));
    QVERIFY(!rng.Contains(INFINITY));

    // Test an exclusive upper bound
    rng = Range<int>::CreateUpperBound(1, false);
    for(int i = -10; i <= 0; i++)
        QVERIFY2(rng.Contains(i), String::FromInt(i));
    for(int i = 1; i <= 10; i++)
        QVERIFY(!rng.Contains(i));
    QVERIFY(rng.Contains(-INFINITY));
    QVERIFY(!rng.Contains(INFINITY));

    // Test an inclusive double bound
    rng = Range<int>::CreateDoubleBound(-1, 1, true, true);
    for(int i = -10; i <= -2; i++)
        QVERIFY2(!rng.Contains(i), String::FromInt(i));
    for(int i = -1; i <= 1; i++)
        QVERIFY(rng.Contains(i));
    for(int i = 2; i <= 10; i++)
        QVERIFY(!rng.Contains(i));
    QVERIFY(!rng.Contains(INFINITY));
    QVERIFY(!rng.Contains(-INFINITY));

    // Test an exclusive double bound
    rng = Range<int>::CreateDoubleBound(-1, 1, false, false);
    for(int i = -10; i <= -1; i++)
        QVERIFY2(!rng.Contains(i), String::FromInt(i));
    for(int i = 0; i <= 0; i++)
        QVERIFY(rng.Contains(i));
    for(int i = 1; i <= 10; i++)
        QVERIFY(!rng.Contains(i));
    QVERIFY(!rng.Contains(INFINITY));
    QVERIFY(!rng.Contains(-INFINITY));

    // Test a double bound with one bound exclusive and the other inclusive
    rng = Range<int>::CreateDoubleBound(-1, 1, true, false);
    for(int i = -10; i <= -2; i++)
        QVERIFY2(!rng.Contains(i), String::FromInt(i));
    for(int i = -1; i <= 0; i++)
        QVERIFY(rng.Contains(i));
    for(int i = 1; i <= 10; i++)
        QVERIFY(!rng.Contains(i));
    QVERIFY(!rng.Contains(INFINITY));
    QVERIFY(!rng.Contains(-INFINITY));

    // Test a double bound with one bound exclusive and the other inclusive
    rng = Range<int>::CreateDoubleBound(-1, 1, false, true);
    for(int i = -10; i <= -1; i++)
        QVERIFY2(!rng.Contains(i), String::FromInt(i));
    for(int i = 0; i <= 1; i++)
        QVERIFY(rng.Contains(i));
    for(int i = 2; i <= 10; i++)
        QVERIFY(!rng.Contains(i));
    QVERIFY(!rng.Contains(INFINITY));
    QVERIFY(!rng.Contains(-INFINITY));

    // Test a double bound with the lower bound greater than the upper bound (Case 1)
    rng = Range<int>::CreateDoubleBound(1, -1, true, true);
    for(int i = -10; i <= -1; i++)
        QVERIFY2(rng.Contains(i), String::FromInt(i));
    for(int i = 0; i <= 0; i++)
        QVERIFY(!rng.Contains(i));
    for(int i = 1; i <= 10; i++)
        QVERIFY(rng.Contains(i));
    QVERIFY(rng.Contains(INFINITY));
    QVERIFY(rng.Contains(-INFINITY));

    // Test a double bound with the lower bound greater than the upper bound (Case 2)
    rng = Range<int>::CreateDoubleBound(1, -1, false, false);
    for(int i = -10; i <= -2; i++)
        QVERIFY2(rng.Contains(i), String::FromInt(i));
    for(int i = -1; i <= 1; i++)
        QVERIFY(!rng.Contains(i));
    for(int i = 2; i <= 10; i++)
        QVERIFY(rng.Contains(i));
    QVERIFY(rng.Contains(INFINITY));
    QVERIFY(rng.Contains(-INFINITY));

    // Test a double bound with the lower bound greater than the upper bound (Case 3)
    rng = Range<int>::CreateDoubleBound(1, -1, true, false);
    for(int i = -10; i <= -2; i++)
        QVERIFY2(rng.Contains(i), String::FromInt(i));
    for(int i = -1; i <= 0; i++)
        QVERIFY(!rng.Contains(i));
    for(int i = 1; i <= 10; i++)
        QVERIFY(rng.Contains(i));
    QVERIFY(rng.Contains(INFINITY));
    QVERIFY(rng.Contains(-INFINITY));

    // Test a double bound with the lower bound greater than the upper bound (Case 4)
    rng = Range<int>::CreateDoubleBound(1, -1, false, true);
    for(int i = -10; i <= -1; i++)
        QVERIFY2(rng.Contains(i), String::FromInt(i));
    for(int i = 0; i <= 1; i++)
        QVERIFY(!rng.Contains(i));
    for(int i = 2; i <= 10; i++)
        QVERIFY(rng.Contains(i));
    QVERIFY(rng.Contains(INFINITY));
    QVERIFY(rng.Contains(-INFINITY));


    // Test some special ranges

    // Test the universe range for a bunch of values
    rng = Range<int>::Universe();
    for(int i = -100; i <= 100; ++i)
        QVERIFY(rng.Contains(i));
    QVERIFY(rng.Contains(-INFINITY));
    QVERIFY(rng.Contains(INFINITY));

    // Test a single-value range, created by a range with initialized lower and upper bounds
    rng = Range<int>::CreateDoubleBound(0, 0, true, true);
    QVERIFY(!rng.IsUniverse());
    QVERIFY(rng.HasLowerBound());
    QVERIFY(rng.HasUpperBound());
    QVERIFY(!rng.Contains(-1));
    QVERIFY(rng.Contains(0));
    QVERIFY(!rng.Contains(1));

    // This case doesn't result in a Universe set, because it excludes the value 0
    rng = Range<int>::CreateDoubleBound(0, 0, false, false);
    QVERIFY(!rng.IsUniverse());
    QVERIFY(rng.HasLowerBound());
    QVERIFY(rng.HasUpperBound());
    QVERIFY(rng.Contains(-1));
    QVERIFY(!rng.Contains(0));
    QVERIFY(rng.Contains(1));
    QVERIFY(rng.Contains(INFINITY));
    QVERIFY(rng.Contains(-INFINITY));

    // By convention the following two scenarios are interpreted as a Universe Set
    rng = Range<int>::CreateDoubleBound(0, 0, true, false);
    QVERIFY(rng.IsUniverse());

    rng = Range<int>::CreateDoubleBound(0, 0, false, true);
    QVERIFY(rng.IsUniverse());
}

void RangeTest::test_range_intersects()
{
    Range<int> r1;
    Range<int> r2;

    // Null ranges cannot intersect
    QVERIFY(!r1.Intersects(r2));
    QVERIFY(!r2.Intersects(r1));

    r1 = Range<int>::CreateLowerBound(10);
    QVERIFY(!r1.Intersects(r2));
    QVERIFY(!r2.Intersects(r1));

    // Null ranges cannot intersect (even with the Universe)
    r1 = Range<int>::Universe();
    QVERIFY(!r1.Intersects(r2));
    QVERIFY(!r2.Intersects(r1));

    // The Universe intersects with anything not-null
    r2 = Range<int>::CreateLowerBound(5);
    QVERIFY(r1.Intersects(r2));
    QVERIFY(r2.Intersects(r1));


    // Now test all the possible intersecting cases (and their argument-inverses):

    // |-->
    //      |-->
    r1 = Range<int>::CreateLowerBound(0);
    r2 = Range<int>::CreateLowerBound(10);
    QVERIFY(Range<int>::Intersects(r1, r2));
    QVERIFY(Range<int>::Intersects(r2, r1));

    // |-->
    //    <--|
    r1 = Range<int>::CreateLowerBound(0);
    r2 = Range<int>::CreateUpperBound(10);
    QVERIFY(Range<int>::Intersects(r1, r2));
    QVERIFY(Range<int>::Intersects(r2, r1));

    // |-->
    //    |--|
    r1 = Range<int>::CreateLowerBound(0);
    r2 = Range<int>::CreateDoubleBound(2, 5);
    QVERIFY(Range<int>::Intersects(r1, r2));
    QVERIFY(Range<int>::Intersects(r2, r1));

    //    |-->
    //  |---|
    r1 = Range<int>::CreateLowerBound(3);
    r2 = Range<int>::CreateDoubleBound(0, 5);
    QVERIFY(Range<int>::Intersects(r1, r2));
    QVERIFY(Range<int>::Intersects(r2, r1));

    //      <--|
    //  |---|
    r1 = Range<int>::CreateUpperBound(10);
    r2 = Range<int>::CreateDoubleBound(0, 5);
    QVERIFY(Range<int>::Intersects(r1, r2));
    QVERIFY(Range<int>::Intersects(r2, r1));

    //  <---|
    //    |---|
    r1 = Range<int>::CreateUpperBound(3);
    r2 = Range<int>::CreateDoubleBound(0, 5);
    QVERIFY(Range<int>::Intersects(r1, r2));
    QVERIFY(Range<int>::Intersects(r2, r1));

    //  |---|
    //    |---|
    r1 = Range<int>::CreateDoubleBound(0, 3, true, true);
    r2 = Range<int>::CreateDoubleBound(3, 5, true, true);
    QVERIFY(Range<int>::Intersects(r1, r2));
    QVERIFY(Range<int>::Intersects(r2, r1));

    //  |-------|
    //    |---|
    r1 = Range<int>::CreateDoubleBound(0, 10);
    r2 = Range<int>::CreateDoubleBound(3, 7);
    QVERIFY(Range<int>::Intersects(r1, r2));
    QVERIFY(Range<int>::Intersects(r2, r1));



    // Now test all possible non-intersecting cases

    //       |-->
    //  <--|
    r1 = Range<int>::CreateLowerBound(0, true);
    r2 = Range<int>::CreateUpperBound(0, false);
    QVERIFY(!Range<int>::Intersects(r1, r2));
    QVERIFY(!Range<int>::Intersects(r2, r1));

    //      |-->
    // |--|
    r1 = Range<int>::CreateLowerBound(5);
    r2 = Range<int>::CreateDoubleBound(0, 3);
    QVERIFY(!Range<int>::Intersects(r1, r2));
    QVERIFY(!Range<int>::Intersects(r2, r1));

    //  <--|
    //       |--|
    r1 = Range<int>::CreateUpperBound(0);
    r2 = Range<int>::CreateDoubleBound(2, 5);
    QVERIFY(!Range<int>::Intersects(r1, r2));
    QVERIFY(!Range<int>::Intersects(r2, r1));

    //  |---|
    //        |---|
    r1 = Range<int>::CreateDoubleBound(0, 10);
    r2 = Range<int>::CreateDoubleBound(15, 20);
    QVERIFY(!Range<int>::Intersects(r1, r2));
    QVERIFY(!Range<int>::Intersects(r2, r1));
}

void RangeTest::test_range_subset()
{
    // Test the range subset function, which is implemented differently than
    //  the region subset function
    Range<int> r1( Range<int>::CreateDoubleBound(-10, 10) );
    Range<int> r2( Range<int>::CreateDoubleBound(-2, 2));

    QVERIFY(r2.IsSubset(r1));
    QVERIFY(r2 <= r1);  // Another way to say "IsSubset()"
    QVERIFY(r1.IsSuperset(r2));
    QVERIFY(r1 >= r2);  // Another way to say "IsSuperset()"


    // Try with equal ranges
    r1 = Range<int>::CreateDoubleBound(-5, 5);
    r2 = Range<int>::CreateDoubleBound(-5, 5);
    QVERIFY(r1 <= r2);
    QVERIFY(r1 >= r2);
    QVERIFY(r1 == r2);
    QVERIFY(r2 <= r1);
    QVERIFY(r2 >= r1);
    QVERIFY(r2 == r1);

    r2 = Range<int>::CreateDoubleBound(-5, 5, false, true);
    QVERIFY(!(r1 <= r2));
    QVERIFY(r1 >= r2);
    QVERIFY(r1 != r2);
    QVERIFY(r2 <= r1);
    QVERIFY(!(r2 >= r1));
    QVERIFY(r2 != r1);

    r2 = Range<int>::CreateDoubleBound(-5, 5, true, false);
    QVERIFY(!(r1 <= r2));
    QVERIFY(r1 >= r2);
    QVERIFY(r1 != r2);
    QVERIFY(r2 <= r1);
    QVERIFY(!(r2 >= r1));
    QVERIFY(r2 != r1);

    r2 = Range<int>::CreateDoubleBound(-5, 5, false, false);
    QVERIFY(!(r1 <= r2));
    QVERIFY(r1 >= r2);
    QVERIFY(r1 != r2);
    QVERIFY(r2 <= r1);
    QVERIFY(!(r2 >= r1));
    QVERIFY(r2 != r1);


    // Now try some more complex subsets
    r1 = Range<int>::CreateLowerBound(10);
    r2 = Range<int>::CreateDoubleBound(15, 20, true, true);
    QVERIFY(r2 <= r1);
    QVERIFY(!(r1 <= r2));

    r1 = Range<int>::CreateLowerBound(15, true);
    QVERIFY(r2 <= r1);
    QVERIFY(!(r1 <= r2));

    r1 = Range<int>::CreateLowerBound(15, false);
    QVERIFY(!(r2 <= r1));
    QVERIFY(!(r1 <= r2));

    r1 = Range<int>::CreateLowerBound(16, true);
    QVERIFY(!(r2 <= r1));
    QVERIFY(!(r1 <= r2));


    // Try a couple of inverted ranges
    r1 = Range<int>::CreateDoubleBound(10, -10);
    r2 = Range<int>::CreateDoubleBound(20, -20);
    QVERIFY(r2 <= r1);
    QVERIFY(!(r1 <= r2));

    r2 = Range<int>::CreateDoubleBound(5, -20);
    QVERIFY(!(r2 <= r1));
    QVERIFY(!(r1 <= r2));

    r2 = Range<int>::CreateLowerBound(10);
    QVERIFY(r2 <= r1);
    QVERIFY(!(r1 <= r2));
}

void RangeTest::test_range_complement()
{
    // Test that the complementing of ranges behaves as expected
    Range<int> rng( Range<int>::CreateLowerBound(5, true) );
    QVERIFY(rng.HasLowerBound());
    QVERIFY(!rng.HasUpperBound());
    QVERIFY(rng.LowerBound().Value() == 5);
    QVERIFY(rng.LowerBound().ValueIncludedInBound() == true);

    Range<int> cmp( rng.Complemented() );
    QVERIFY(!cmp.HasLowerBound());
    QVERIFY(cmp.HasUpperBound());
    QVERIFY(cmp.UpperBound().Value() == 5);
    QVERIFY(cmp.UpperBound().ValueIncludedInBound() == false);
    QVERIFY(rng.LowerBound().Value() == 5);
    QVERIFY(rng.LowerBound().ValueIncludedInBound() == true);


    rng = Range<int>::CreateUpperBound(5, false);
    QVERIFY(!rng.HasLowerBound());
    QVERIFY(rng.HasUpperBound());
    QVERIFY(rng.UpperBound().Value() == 5);
    QVERIFY(rng.UpperBound().ValueIncludedInBound() == false);

    cmp = ~rng;
    QVERIFY(cmp.HasLowerBound());
    QVERIFY(!cmp.HasUpperBound());
    QVERIFY(cmp.LowerBound().Value() == 5);
    QVERIFY(cmp.LowerBound().ValueIncludedInBound() == true);
    QVERIFY(rng.UpperBound().Value() == 5);
    QVERIFY(rng.UpperBound().ValueIncludedInBound() == false);


    rng = Range<int>::CreateDoubleBound(-5, 5, true, true);
    cmp = ~rng;
    QVERIFY(cmp.HasLowerBound());
    QVERIFY(cmp.HasUpperBound());
    QVERIFY(cmp.LowerBound().Value() == 5);
    QVERIFY(cmp.UpperBound().Value() == -5);
    QVERIFY(cmp.LowerBound().ValueIncludedInBound() == false);
    QVERIFY(cmp.UpperBound().ValueIncludedInBound() == false);
    QVERIFY(rng.LowerBound().Value() == -5);
    QVERIFY(rng.UpperBound().Value() == 5);
    QVERIFY(rng.LowerBound().ValueIncludedInBound() == true);
    QVERIFY(rng.UpperBound().ValueIncludedInBound() == true);
}


void RangeTest::test_unions()
{
    Region<int> reg1, reg2, reg3;
    Range<int> r1, r2, r3;

    // First test bounded unions

    /*  |---|
                |---|    */
    r1 = Range<int>::CreateDoubleBound(0, 10);
    r2 = Range<int>::CreateDoubleBound(15, 20);
    QVERIFY(r1.IsBounded());
    QVERIFY(r2.IsBounded());
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY(reg1.Ranges().Count() == 2);
    QVERIFY(reg2.Ranges().Count() == 2);
    for(int i = 0; i <= 10; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 11; i <= 14; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 15; i <= 20; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(-INFINITY));
    QVERIFY(!reg1.Contains(INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));
    QVERIFY(!reg2.Contains(INFINITY));

    r1 = Range<int>::CreateDoubleBound(0, 10, true, false);
    r2 = Range<int>::CreateDoubleBound(10, 20, false, true);
    QVERIFY(r1.IsBounded());
    QVERIFY(r2.IsBounded());
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    //print_ranges(reg1);
    QVERIFY(reg1.Ranges().Count() == 2);
    QVERIFY(reg2.Ranges().Count() == 2);
    for(int i = 0; i < 10; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY2(reg2.Contains(i), String::FromInt(i));
    }
    QVERIFY(!reg1.Contains(10));
    QVERIFY(!reg2.Contains(10));
    for(int i = 11; i <= 20; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(-INFINITY));
    QVERIFY(!reg1.Contains(INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));
    QVERIFY(!reg2.Contains(INFINITY));


    /*  |---|
          |---|     */
    r1 = Range<int>::CreateDoubleBound(0, 10);
    r2 = Range<int>::CreateDoubleBound(5, 15);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    //print_ranges(reg1);
    QVERIFY2(reg1.Ranges().Size() == 1, String::FromInt(reg1.Ranges().Size()));
    QVERIFY2(reg2.Ranges().Size() == 1, String::FromInt(reg2.Ranges().Size()));
    for(int i = -5; i <= -1; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 0; i <= 15; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 16; i <= 25; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(-INFINITY));
    QVERIFY(!reg1.Contains(INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));
    QVERIFY(!reg2.Contains(INFINITY));

    r1 = Range<int>::CreateDoubleBound(0, 5, true, true);
    r2 = Range<int>::CreateDoubleBound(5, 10, false, true);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY2(reg1.Ranges().Size() == 1, String::FromInt(reg1.Ranges().Size()));
    QVERIFY2(reg2.Ranges().Size() == 1, String::FromInt(reg2.Ranges().Size()));
    for(int i = -5; i <= -1; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 0; i <= 10; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 11; i <= 25; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(-INFINITY));
    QVERIFY(!reg1.Contains(INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));
    QVERIFY(!reg2.Contains(INFINITY));


    /*  |---|
         |-|       */
    r1 = Range<int>::CreateDoubleBound(0, 10);
    r2 = Range<int>::CreateDoubleBound(2, 8);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY(reg1.Ranges().Size() == 1);
    QVERIFY(reg2.Ranges().Size() == 1);
    for(int i = -5; i <= -1; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 0; i <= 10; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 11; i <= 20; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(-INFINITY));
    QVERIFY(!reg1.Contains(INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));
    QVERIFY(!reg2.Contains(INFINITY));



    // Then test one bounded range, one unbounded
    /*    |---->
               |---|    */
    r1 = Range<int>::CreateLowerBound(0);
    r2 = Range<int>::CreateDoubleBound(5, 10);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY(reg1.Ranges().Count() == 1);
    QVERIFY(reg2.Ranges().Count() == 1);
    for(int i = -20; i <= -1; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 0; i <= 100; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(-INFINITY));
    QVERIFY(reg1.Contains(INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));
    QVERIFY(reg2.Contains(INFINITY));

    /*        |---->
       |---|            */
    r1 = Range<int>::CreateLowerBound(10);
    r2 = Range<int>::CreateDoubleBound(0, 5);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY(reg1.Ranges().Count() == 2);
    QVERIFY(reg2.Ranges().Count() == 2);
    for(int i = -20; i <= -1; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 0; i <= 5; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 6; i <= 9; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 10; i <= 100; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(-INFINITY));
    QVERIFY(reg1.Contains(INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));
    QVERIFY(reg2.Contains(INFINITY));

    /*        |---->
            |---|       */
    r1 = Range<int>::CreateLowerBound(5);
    r2 = Range<int>::CreateDoubleBound(0, 10);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY(reg1.Ranges().Count() == 1);
    QVERIFY(reg2.Ranges().Count() == 1);
    for(int i = -20; i <= -1; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 0; i <= 100; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(-INFINITY));
    QVERIFY(reg1.Contains(INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));
    QVERIFY(reg2.Contains(INFINITY));


    /*        <----|
            |---|       */
    r1 = Range<int>::CreateUpperBound(0);
    r2 = Range<int>::CreateDoubleBound(-10, -5);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY(reg1.Ranges().Count() == 1);
    QVERIFY(reg2.Ranges().Count() == 1);
    for(int i = -20; i <= 0; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 1; i <= 100; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(INFINITY));
    QVERIFY(reg1.Contains(-INFINITY));
    QVERIFY(!reg2.Contains(INFINITY));
    QVERIFY(reg2.Contains(-INFINITY));

    /* <----|
               |---|    */
    r1 = Range<int>::CreateUpperBound(0);
    r2 = Range<int>::CreateDoubleBound(5, 10);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    //print_ranges(reg1);
    QVERIFY(reg1.Ranges().Count() == 2);
    QVERIFY(reg2.Ranges().Count() == 2);
    for(int i = -20; i <= 0; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 1; i < 5; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 5; i <= 10; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 11; i <= 100; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(INFINITY));
    QVERIFY(reg1.Contains(-INFINITY));
    QVERIFY(!reg2.Contains(INFINITY));
    QVERIFY(reg2.Contains(-INFINITY));


    /* <----|
          |---|         */
    r1 = Range<int>::CreateUpperBound(5);
    r2 = Range<int>::CreateDoubleBound(0, 10);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY(reg1.Ranges().Count() == 1);
    QVERIFY(reg2.Ranges().Count() == 1);
    for(int i = -20; i <= 10; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 11; i <= 100; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(INFINITY));
    QVERIFY(reg1.Contains(-INFINITY));
    QVERIFY(!reg2.Contains(INFINITY));
    QVERIFY(reg2.Contains(-INFINITY));


    // Now try some unions with both ranges unbounded

    /* |--->
             <---|   */
    r1 = Range<int>::CreateLowerBound(0);
    r2 = Range<int>::CreateUpperBound(5);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    //print_ranges(reg1);
    QVERIFY(reg1.Ranges().Count() == 0);
    QVERIFY(reg2.Ranges().Count() == 0);
    QVERIFY(reg1.IsUniverse());
    QVERIFY(reg2.IsUniverse());
    QVERIFY(reg1.Contains(INFINITY));
    QVERIFY(reg1.Contains(-INFINITY));
    QVERIFY(reg2.Contains(INFINITY));
    QVERIFY(reg2.Contains(-INFINITY));

    /*             |--->
          <---|                 */
    r1 = Range<int>::CreateLowerBound(5);
    r2 = Range<int>::CreateUpperBound(0);
    r3 = Range<int>::CreateDoubleBound(5, 0);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    reg3 = r3;
    QVERIFY(reg1.Ranges().Count() == 2);
    QVERIFY(reg2.Ranges().Count() == 2);
    QVERIFY(reg3.Ranges().Count() == 2);
    QVERIFY(!reg1.IsUniverse());
    QVERIFY(!reg2.IsUniverse());
    QVERIFY(!reg3.IsUniverse());
    QVERIFY(reg1.Contains(INFINITY));
    QVERIFY(reg1.Contains(-INFINITY));
    QVERIFY(reg2.Contains(INFINITY));
    QVERIFY(reg2.Contains(-INFINITY));
    QVERIFY(reg3.Contains(INFINITY));
    QVERIFY(reg3.Contains(-INFINITY));
    for(int i = -20; i <= 0; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
        QVERIFY(reg3.Contains(i));
    }
    for(int i = 1; i < 5; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
        QVERIFY(!reg3.Contains(i));
    }
    for(int i = 5; i <= 20; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
        QVERIFY(reg3.Contains(i));
    }


    // Now test Unions of more complex regions

    //    <--|   |-->
    //      |-----|
    r1 = Range<int>::CreateDoubleBound(8, 2);
    r2 = Range<int>::CreateDoubleBound(0, 10);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY(reg1.IsUniverse());
    QVERIFY(reg2.IsUniverse());
    QVERIFY(reg1.Ranges().Count() == 0);
    QVERIFY(reg2.Ranges().Count() == 0);

    r1 = Range<int>::CreateDoubleBound(10, 0, true, true);
    r2 = Range<int>::CreateDoubleBound(0, 10, false, false);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY(reg1.IsUniverse());
    QVERIFY(reg2.IsUniverse());
    QVERIFY(reg1.Ranges().Count() == 0);
    QVERIFY(reg2.Ranges().Count() == 0);

    r1 = Range<int>::CreateDoubleBound(10, 0, false, false);
    r2 = Range<int>::CreateDoubleBound(0, 10, true, true);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    QVERIFY(reg1.IsUniverse());
    QVERIFY(reg2.IsUniverse());
    QVERIFY(reg1.Ranges().Count() == 0);
    QVERIFY(reg2.Ranges().Count() == 0);

    // <---|          |--->
    //         |--|
    r1 = Range<int>::CreateDoubleBound(10, 0);
    r2 = Range<int>::CreateDoubleBound(4, 6);
    reg1 = r1 + r2;
    reg2 = r2 + r1;
    //print_ranges(reg1);
    QVERIFY(reg1.Ranges().Count() == 3);
    QVERIFY(reg2.Ranges().Count() == 3);
    int cnt = 0;
    for(DList< Range<int> >::const_iterator iter(reg1.Ranges().begin());
        iter != reg1.Ranges().end(); ++iter, ++cnt)
    {
        if(0 == cnt){
            QVERIFY(iter->LowerBound().IsNull());
            QVERIFY(iter->UpperBound().Value() == 0);
        }
        else if(1 == cnt){
            QVERIFY(iter->LowerBound().Value() == 4);
            QVERIFY(iter->UpperBound().Value() == 6);
        }
        else if(2 == cnt){
            QVERIFY(iter->LowerBound().Value() == 10);
            QVERIFY(iter->UpperBound().IsNull());
        }
    }
    cnt = 0;
    for(DList< Range<int> >::const_iterator iter(reg2.Ranges().begin());
        iter != reg2.Ranges().end(); ++iter, ++cnt)
    {
        if(0 == cnt){
            QVERIFY(iter->LowerBound().IsNull());
            QVERIFY(iter->UpperBound().Value() == 0);
        }
        else if(1 == cnt){
            QVERIFY(iter->LowerBound().Value() == 4);
            QVERIFY(iter->UpperBound().Value() == 6);
        }
        else if(2 == cnt){
            QVERIFY(iter->LowerBound().Value() == 10);
            QVERIFY(iter->UpperBound().IsNull());
        }
    }

    // |---|        |---|        |-->
    //        |--|     |----------|             |--|
    reg3 = Range<int>::CreateDoubleBound(0, 5) +
            Range<int>::CreateDoubleBound(30, 40) +
            Range<int>::CreateLowerBound(100);
    reg2 = Range<int>::CreateDoubleBound(10, 20) +
            Range<int>::CreateDoubleBound(35, 100) +
            Range<int>::CreateDoubleBound(150, 175);
    reg1 = reg3 + reg2;
    reg2 = reg2 + reg3;
    //print_ranges(reg1);
    //print_ranges(reg2);
    QVERIFY2(reg1.Ranges().Count() == 3, String::FromInt(reg1.Ranges().Count()));
    QVERIFY2(reg2.Ranges().Count() == 3, String::FromInt(reg2.Ranges().Count()));
    for(int i = -10; i < 0; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 0; i <= 5; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 6; i < 10; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 10; i <= 20; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 21; i < 30; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 30; i <= 200; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    QVERIFY(reg1.Contains(INFINITY));
    QVERIFY(!reg1.Contains(-INFINITY));
    QVERIFY(reg2.Contains(INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));

    // |---|        |---|                |---|
    //     |--------|     |---|   |--|
    reg3 = Range<int>::CreateDoubleBound(0, 5) +
            Range<int>::CreateDoubleBound(30, 40) +
            Range<int>::CreateDoubleBound(80, 90);
    reg2 = Range<int>::CreateDoubleBound(5, 30) +
            Range<int>::CreateDoubleBound(50, 60) +
            Range<int>::CreateDoubleBound(65, 70);
    reg1 = reg3 + reg2;
    reg2 = reg2 + reg3;
    QVERIFY2(reg1.Ranges().Count() == 4, String::FromInt(reg1.Ranges().Count()));
    QVERIFY2(reg2.Ranges().Count() == 4, String::FromInt(reg2.Ranges().Count()));
    for(int i = -10; i < 0; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 0; i <= 40; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 41; i < 50; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 50; i <= 60; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 61; i < 65; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 65; i <= 70; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 71; i < 80; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 80; i <= 90; i++){
        QVERIFY(reg1.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 91; i < 100; i++){
        QVERIFY(!reg1.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    QVERIFY(!reg1.Contains(-INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));
    QVERIFY(!reg1.Contains(INFINITY));
    QVERIFY(!reg2.Contains(INFINITY));
}


void RangeTest::test_region_complement()
{
    Region<int> reg, cmp, reg2;

    /*    |--->   */
    reg = Range<int>::CreateLowerBound(0);
    cmp = reg.Complemented();
    QVERIFY(cmp.Ranges().Count() == 1);
    for(int i = -10; i <= -1; i++){
        QVERIFY(cmp.Contains(i));
        QVERIFY(!reg.Contains(i));
    }
    for(int i = 0; i <= 10; i++){
        QVERIFY(!cmp.Contains(i));
        QVERIFY(reg.Contains(i));
    }
    QVERIFY(cmp.Contains(-INFINITY));
    QVERIFY(!cmp.Contains(INFINITY));
    QVERIFY(reg.Contains(INFINITY));
    QVERIFY(!reg.Contains(-INFINITY));

    /*    <---|   */
    reg = Range<int>::CreateUpperBound(0);
    cmp = reg.Complemented();
    QVERIFY(cmp.Ranges().Count() == 1);
    for(int i = -10; i <= 0; i++){
        QVERIFY(!cmp.Contains(i));
        QVERIFY(reg.Contains(i));
    }
    for(int i = 1; i <= 10; i++){
        QVERIFY(cmp.Contains(i));
        QVERIFY(!reg.Contains(i));
    }
    QVERIFY(!cmp.Contains(-INFINITY));
    QVERIFY(cmp.Contains(INFINITY));
    QVERIFY(!reg.Contains(INFINITY));
    QVERIFY(reg.Contains(-INFINITY));

    /*    |--|   */
    reg = Range<int>::CreateDoubleBound(-5, 5);
    cmp = ~reg;
    QVERIFY(cmp.Ranges().Count() == 2);
    for(int i = -10; i <= -6; i++){
        QVERIFY(cmp.Contains(i));
        QVERIFY(!reg.Contains(i));
    }
    for(int i = -5; i <= 5; i++){
        QVERIFY(!cmp.Contains(i));
        QVERIFY(reg.Contains(i));
    }
    for(int i = 6; i <= 10; i++){
        QVERIFY(cmp.Contains(i));
        QVERIFY(!reg.Contains(i));
    }
    QVERIFY(cmp.Contains(-INFINITY));
    QVERIFY(cmp.Contains(INFINITY));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));


    /*    <--->   */
    reg = Range<int>::Universe();
    cmp = reg.Complemented();
    QVERIFY(cmp.IsEmpty());

    reg = Region<int>::Universe();
    cmp = reg.Complemented();
    QVERIFY(cmp.IsEmpty());

    // ( null range )
    reg = Range<int>::EmptySet();
    cmp = reg.Complemented();
    QVERIFY(cmp.IsUniverse());

    reg = Region<int>::EmptySet();
    cmp = reg.Complemented();
    QVERIFY(cmp.IsUniverse());


    /*    |---|   |--->   becomes...   <---)   (----)    */
    reg = Range<int>::CreateDoubleBound(0, 5) + Range<int>::CreateLowerBound(10);
    cmp = ~reg;
    reg2 = ~cmp;
//    print_ranges(cmp);
//    print_ranges(reg2);
    for(int i = -20; i < 0; i++){
        QVERIFY(!reg.Contains(i));
        QVERIFY(cmp.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 0; i <= 5; i++){
        QVERIFY(reg.Contains(i));
        QVERIFY(!cmp.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 6; i < 10; i++){
        QVERIFY(!reg.Contains(i));
        QVERIFY(cmp.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 10; i <= 30; i++){
        QVERIFY(reg.Contains(i));
        QVERIFY(!cmp.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    QVERIFY(reg.Contains(INFINITY));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(cmp.Contains(-INFINITY));
    QVERIFY(!cmp.Contains(INFINITY));
    QVERIFY(reg2.Contains(INFINITY));
    QVERIFY(!reg2.Contains(-INFINITY));


    /*    |---|   |---|  becomes:   <---)   (--)   (--->   */
    reg = Range<int>::CreateDoubleBound(0, 5) + Range<int>::CreateDoubleBound(10, 15);
    cmp = ~reg;
    reg2 = ~cmp;
    //print_ranges(reg);
    //print_ranges(cmp);
    //print_ranges(reg2);
    QVERIFY(reg.Ranges().Count() == 2);
    QVERIFY(cmp.Ranges().Count() == 3);
    QVERIFY(reg2.Ranges().Count() == 2);
    for(int i = -20; i < 0; i++){
        QVERIFY(!reg.Contains(i));
        QVERIFY(cmp.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 0; i <= 5; i++){
        QVERIFY(reg.Contains(i));
        QVERIFY(!cmp.Contains(i));
        QVERIFY(reg2.Contains(i));
    }
    for(int i = 6; i < 10; i++){
        QVERIFY(!reg.Contains(i));
        QVERIFY(cmp.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    for(int i = 10; i <= 15; i++){
        QVERIFY(reg.Contains(i));
        QVERIFY(!cmp.Contains(i));
        QVERIFY(reg.Contains(i));
    }
    for(int i = 16; i <= 30; i++){
        QVERIFY(!reg.Contains(i));
        QVERIFY(cmp.Contains(i));
        QVERIFY(!reg2.Contains(i));
    }
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));
    QVERIFY(cmp.Contains(-INFINITY));
    QVERIFY(cmp.Contains(INFINITY));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));
}

void RangeTest::test_intersect()
{
    Region<int> intersect;

    // Test the no-brainers first

    // Universe & X == X
    intersect = Region<int>::Universe() & Range<int>::CreateDoubleBound(0, 5);
    QVERIFY(!intersect.IsEmpty());
    QVERIFY(intersect.Ranges().Count() == 1);
    for(int i = -10; i < 0; ++i)
        QVERIFY(!intersect.Contains(i));
    for(int i = 0; i <= 5; ++i)
        QVERIFY(intersect.Contains(i));
    for(int i = 6; i < 10; ++i)
        QVERIFY(!intersect.Contains(i));


    // EmptySet & X == EmptySet
    intersect = Region<int>::EmptySet() & Range<int>::CreateDoubleBound(0, 5);
    QVERIFY(intersect.IsEmpty());

    intersect = Region<int>::EmptySet() & Region<int>::Universe();
    QVERIFY(intersect.IsEmpty());


    /*    |---|
                    |---|     */
    intersect = Range<int>::CreateDoubleBound(0, 5) & Range<int>::CreateDoubleBound(10, 15);
    QVERIFY(intersect.IsEmpty());


    /*    |---|
             |---|     */
    intersect = Range<int>::CreateDoubleBound(0, 5) & Range<int>::CreateDoubleBound(3, 10);
    QVERIFY(!intersect.IsEmpty());
    QVERIFY(!intersect.Contains(2));
    QVERIFY(intersect.Contains(3));
    QVERIFY(intersect.Contains(4));
    QVERIFY(intersect.Contains(5));
    QVERIFY(!intersect.Contains(6));


    /*    |---|
              |---|     */
    intersect = Range<int>::CreateDoubleBound(0, 5) & Range<int>::CreateDoubleBound(5, 10);
    //print_ranges(intersect);
    QVERIFY(!intersect.IsEmpty());
    QVERIFY(!intersect.Contains(4));
    QVERIFY(intersect.Contains(5));
    QVERIFY(!intersect.Contains(6));


    /*    |---)
              (---|     */
    intersect = Range<int>::CreateDoubleBound(0, 5, true, false) & Range<int>::CreateDoubleBound(5, 10, false, true);
    //print_ranges(intersect);
    QVERIFY(intersect.IsEmpty());


    /*    |--->
             <---|     */
    intersect = Range<int>::CreateLowerBound(0) & Range<int>::CreateUpperBound(5);
    //print_ranges(intersect);
    QVERIFY(!intersect.IsEmpty());
    QVERIFY(intersect.Ranges().Count() == 1);
    for(int i = -20; i < 0; i++)
        QVERIFY(!intersect.Contains(i));
    for(int i = 0; i <= 5; i++)
        QVERIFY(intersect.Contains(i));
    for(int i = 6; i <= 20; i++)
        QVERIFY(!intersect.Contains(i));

    /*          |--->
        <---|               */
    intersect = Range<int>::CreateLowerBound(5) & Range<int>::CreateUpperBound(-5);
    //print_ranges(intersect);
    QVERIFY(intersect.IsEmpty());


    // Now try some more complex intersects
    //     |------------|
    //   |---|        |---|
    intersect =
            Range<int>::CreateDoubleBound(5, 15) &
            (Range<int>::CreateDoubleBound(0, 8) + Range<int>::CreateDoubleBound(12, 20));
    //print_ranges(intersect);
    QVERIFY(!intersect.IsEmpty());
    QVERIFY(intersect.Ranges().Count() == 2);
    for(int i = -20; i < 5; i++)
        QVERIFY(!intersect.Contains(i));
    for(int i = 5; i <= 8; i++)
        QVERIFY(intersect.Contains(i));
    for(int i = 9; i < 12; i++)
        QVERIFY(!intersect.Contains(i));
    for(int i = 12; i <= 15; i++)
        QVERIFY(intersect.Contains(i));
    for(int i = 16; i < 20; i++)
        QVERIFY(!intersect.Contains(i));

    //       (------------)
    //   |---|            |---|
    intersect =
            Range<int>::CreateDoubleBound(5, 15, false, false) &
            (Range<int>::CreateDoubleBound(0, 5) + Range<int>::CreateDoubleBound(15, 20));
    //print_ranges(intersect);
    QVERIFY(intersect.IsEmpty());
}

void RangeTest::test_difference()
{
    Region<int> diff;

    // |------->
    //      |------->
    diff = Range<int>::CreateLowerBound(0) - Range<int>::CreateLowerBound(10);
    QVERIFY(diff.Ranges().Count() == 1);
    for(int i = -10; i < 0; i++)
        QVERIFY(!diff.Contains(i));
    for(int i = 0; i < 10; i++)
        QVERIFY(diff.Contains(i));
    for(int i = 10; i < 20; i++)
        QVERIFY(!diff.Contains(i));

    //  |-----------|
    //         |--------->
    diff = Range<int>::CreateDoubleBound(0, 10) - Range<int>::CreateLowerBound(5);
    QVERIFY(diff.Ranges().Count() == 1);
    for(int i = -5; i < 0; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 0; i < 5; ++i)
        QVERIFY(diff.Contains(i));
    for(int i = 5; i < 20; ++i)
        QVERIFY(!diff.Contains(i));

    //  |-----------------|
    //         |----|
    diff = Range<int>::CreateDoubleBound(0, 10) - Range<int>::CreateDoubleBound(3, 7);
    //print_ranges(diff);
    QVERIFY(diff.Ranges().Count() == 2);
    for(int i = -5; i < 0; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 0; i <= 2; ++i)
        QVERIFY(diff.Contains(i));
    for(int i = 3; i <= 7; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 8; i <= 10; ++i)
        QVERIFY(diff.Contains(i));
    for(int i = 11; i < 20; ++i)
        QVERIFY(!diff.Contains(i));

    //   |----|     |----|
    //      |---------|
    diff = (Range<int>::CreateDoubleBound(0, 10) + Range<int>::CreateDoubleBound(20, 30)) -
            Range<int>::CreateDoubleBound(6, 24);
    //print_ranges(diff);
    QVERIFY(diff.Ranges().Count() == 2);
    for(int i = -5; i < 0; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 0; i <= 5; ++i)
        QVERIFY(diff.Contains(i));
    for(int i = 6; i <= 24; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 25; i <= 30; ++i)
        QVERIFY(diff.Contains(i));
    for(int i = 31; i < 40; ++i)
        QVERIFY(!diff.Contains(i));

    //   |---|
    //  |-----|
    diff = Range<int>::CreateDoubleBound(2, 8) - Range<int>::CreateDoubleBound(0, 10);
    QVERIFY(diff.IsEmpty());


    // Try a more complex diff (can't test every case, but we can come up tough ones)
    //     |---|      |-------------|    |------------>
    //   |---------------)                    (--)
    diff =
            ((Range<int>::CreateDoubleBound(5, 10) + Range<int>::CreateDoubleBound(20, 30)) += Range<int>::CreateLowerBound(40)) -
            (Range<int>::CreateDoubleBound(0, 25, true, false) + Range<int>::CreateDoubleBound(45, 50, false, false));
    //print_ranges(diff);
    QVERIFY(diff.Ranges().Count() == 3);
    for(int i = -10; i < 25; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 25; i <= 30; ++i)
        QVERIFY(diff.Contains(i));
    for(int i = 31; i < 40; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 40; i <= 45; ++i)
        QVERIFY(diff.Contains(i));
    for(int i = 46; i < 50; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 50; i < 70; ++i)
        QVERIFY(diff.Contains(i));
}

void RangeTest::test_symmetric_difference()
{
    Region<int> diff;

    /*   |----|
            |----|*/
    diff = Range<int>::CreateDoubleBound(0, 10) ^
            Range<int>::CreateDoubleBound(5, 15);
    //print_ranges(diff);
    QVERIFY(diff.Ranges().Count() == 2);
    for(int i = -20; i <= -1; i++)
        QVERIFY(!diff.Contains(i));
    for(int i = 0; i < 5; i++)
        QVERIFY(diff.Contains(i));
    for(int i = 5; i <= 10; i++)
        QVERIFY(!diff.Contains(i));
    for(int i = 11; i <= 15; i++)
        QVERIFY(diff.Contains(i));
    for(int i = 16; i <= 30; i++)
        QVERIFY(!diff.Contains(i));

    //  |----|       |----|
    //     |-----------|
    diff = (Range<int>::CreateDoubleBound(0, 10) + Range<int>::CreateDoubleBound(20, 30)) ^
            Range<int>::CreateDoubleBound(5, 25);
    //print_ranges(diff);
    QVERIFY(diff.Ranges().Count() == 3);
    for(int i = -10; i < 0; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 0; i <= 4; ++i)
        QVERIFY(diff.Contains(i));
    for(int i = 5; i <= 10; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 11; i <= 19; ++i)
        QVERIFY(diff.Contains(i));
    for(int i = 20; i <= 25; ++i)
        QVERIFY(!diff.Contains(i));
    for(int i = 26; i <= 30; ++i)
        QVERIFY(diff.Contains(i));
    for(int i = 31; i < 40; ++i)
        QVERIFY(!diff.Contains(i));
}



QTEST_APPLESS_MAIN(RangeTest);

#include "range_test.moc"

