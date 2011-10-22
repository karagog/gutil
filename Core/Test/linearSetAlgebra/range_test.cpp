/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

/*Copyright 2011 George Karagoulis

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
#include "Utils/linearsetalgebra.h"
GUTIL_USING_CORE_NAMESPACE(Utils);

class RangeTest :
        public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testBasics();
    void testUnions();
    void testComplement();
    void testSymmetricDifference();
    void testIntercept();

};

void RangeTest::testBasics()
{
    IntegerRegion reg;
    QVERIFY(reg.IsNull());
    QVERIFY(!reg.IsUniverseSet());

    IntegerRange tr1;
    QVERIFY(tr1.IsNull());
    QVERIFY(!tr1.IsUniverse());
    QVERIFY(tr1.IsBounded());

    tr1 = tr1.Universe();
    QVERIFY(!tr1.IsNull());
    QVERIFY(tr1.IsUniverse());
    QVERIFY(!tr1.IsBounded());

    // Test the contains functionality
    reg = IntegerRange(-1, 1);
    for(int i = -10; i <= -2; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = -1; i <= 1; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 2; i <= 10; i++)
        QVERIFY(!reg.Contains(i));

    tr1 = IntegerRange();
    tr1.SetLowerBound(0);
    reg = tr1;
    for(int i = -10; i <= -1; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 0; i <= 10; i++)
        QVERIFY(reg.Contains(i));

    tr1 = IntegerRange();
    tr1.SetUpperBound(0);
    reg = tr1;
    for(int i = -10; i <= 0; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 1; i <= 10; i++)
        QVERIFY(!reg.Contains(i));
}


#define INFINITY 999999

void RangeTest::testUnions()
{
    IntegerRegion reg;
    IntegerRange r1, r2;

    // First test bounded unions
    /*  |---|
                |---|    */
    r1 = IntegerRange(0, 10);
    r2 = IntegerRange(15, 20);
    QVERIFY(r1.IsBounded());
    QVERIFY(r2.IsBounded());
    reg = IntegerRegion::Union(r1, r2);
    for(int i = 0; i <= 10; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 11; i <= 14; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 15; i <= 20; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));

    /*  |---|
          |---|     */
    reg = IntegerRegion::Union(IntegerRange(0, 10), IntegerRange(5, 15));
    QVERIFY(reg.Ranges().size() == 1);
    for(int i = 0; i <= 15; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));

    /*  |---|
         |-|       */
    reg = IntegerRegion::Union(IntegerRange(0, 10), IntegerRange(6, 9));
    QVERIFY(reg.Ranges().size() == 1);
    for(int i = 0; i <= 10; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));

    /*  |---|
      |---|       */
    reg = IntegerRegion::Union(IntegerRange(0, 10), IntegerRange(-5, 5));
    QVERIFY(reg.Ranges().size() == 1);
    for(int i = -5; i <= 10; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));

    /*       |---|
      |---|       */
    reg = IntegerRegion::Union(IntegerRange(0, 10), IntegerRange(-20, -10));
    for(int i = -20; i <= -10; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -9; i <= -1; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 0; i <= 10; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));


    // Then test one bounded range, one unbounded (the implementation of this is
    //  symmetric, so testing one range as unbounded is the same if you reverse
    //  the ranges in the argument list)
    /*    |---->
               |---|    */
    r1 = IntegerRange();
    r1.SetLowerBound(0);
    reg = IntegerRegion::Union(r1, IntegerRange(15, 20));
    for(int i = -20; i <= -1; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 0; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /*        |---->
       |---|            */
    r1 = IntegerRange();
    r1.SetLowerBound(0);
    reg = IntegerRegion::Union(r1, IntegerRange(-25, -20));
    for(int i = -30; i <= -26; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = -25; i <= -20; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -19; i <= -1; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 0; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /*        |---->
            |---|       */
    r1 = IntegerRange();
    r1.SetLowerBound(0);
    reg = IntegerRegion::Union(r1, IntegerRange(-5, 5));
    for(int i = -30; i <= -6; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = -5; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));


    /*        <----|
            |---|       */
    r1 = IntegerRange();
    r1.SetUpperBound(0);
    reg = IntegerRegion::Union(r1, IntegerRange(-20, -15));
    for(int i = -100; i <= 0; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 1; i <= 20; i++)
        QVERIFY(!reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));

    /* <----|
               |---|    */
    r1 = IntegerRange();
    r1.SetUpperBound(0);
    reg = IntegerRegion::Union(r1, IntegerRange(10, 15));
    for(int i = -100; i <= 0; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 1; i <= 9; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 10; i <= 15; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 16; i <= 100; i++)
        QVERIFY(!reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));


    /* <----|
          |---|         */
    r1 = IntegerRange();
    r1.SetUpperBound(0);
    reg = IntegerRegion::Union(r1, IntegerRange(-5, 5));
    for(int i = -100; i <= 5; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 6; i <= 100; i++)
        QVERIFY(!reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));


    // Then a range with both ends unbounded
    /* <----|         |---->
      |---|                 */
    reg = IntegerRegion::Union(IntegerRange(5, -5), IntegerRange(-15, -10));
    for(int i = -100; i <= -5; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -4; i <= 4; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 5; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /* <----|         |---->
           |---|            */
    reg = IntegerRegion::Union(IntegerRange(5, -5), IntegerRange(-10, 0));
    for(int i = -100; i <= 0; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 1; i <= 4; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 5; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /* <----|         |---->
               |---|         */
    reg = IntegerRegion::Union(IntegerRange(5, -5), IntegerRange(-1, 1));
    for(int i = -100; i <= -5; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -4; i <= -2; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = -1; i <= 1; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 2; i <= 4; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 5; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));


    /* <----|         |---->
                    |---|    */
    reg = IntegerRegion::Union(IntegerRange(5, -5), IntegerRange(0, 10));
    for(int i = -100; i <= -5; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -4; i <= -1; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 0; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));


    /* <----|         |---->
                       |---| */
    reg = IntegerRegion::Union(IntegerRange(5, -5), IntegerRange(10, 15));
    for(int i = -100; i <= -5; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -4; i <= 4; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 5; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));


    // The final union tests have both ranges unbounded
    /* <----|         |---->
       <--|             |--> */
    reg = IntegerRegion::Union(IntegerRange(5, -5), IntegerRange(10, -10));
    for(int i = -100; i <= -5; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -4; i <= 4; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 5; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /* <----|         |---->
       <--|          |--> */
    reg = IntegerRegion::Union(IntegerRange(5, -5), IntegerRange(3, -10));
    for(int i = -100; i <= -5; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -4; i <= 2; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 3; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /* <----|         |---->
           <--|     |-->     */
    reg = IntegerRegion::Union(IntegerRange(5, -5), IntegerRange(3, -3));
    for(int i = -100; i <= -3; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -2; i <= 2; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 3; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /* <----|         |---->
           <--|         |-->     */
    reg = IntegerRegion::Union(IntegerRange(5, -5), IntegerRange(10, -3));
    for(int i = -100; i <= -3; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -2; i <= 4; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 5; i <= 100; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /* |--->
             <---|   */
    r1 = r2 = IntegerRange();
    r1.SetLowerBound(0);
    r2.SetUpperBound(1);
    reg = IntegerRegion::Union(r1, r2);
    QVERIFY(reg.IsUniverseSet());

    /*             |--->
          <---|                 */
    r1 = r2 = IntegerRange();
    r1.SetLowerBound(5);
    r2.SetUpperBound(-5);
    reg = IntegerRegion::Union(r1, r2);
    for(int i = -20; i <= -5; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -4; i <= 4; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 5; i <= 20; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /* <----|   |---->
                      <---|    |--->     */
    reg = IntegerRegion::Union(IntegerRange(5, -5), IntegerRange(25, 15));
    QVERIFY(reg.IsUniverseSet());

    /*              <----|   |---->
         <---|    |--->                  */
    reg = IntegerRegion::Union(IntegerRange(25, 15), IntegerRange(5, -5));
    QVERIFY(reg.IsUniverseSet());

    /*   <----|   |---->
             <---|    |--->   */
    // This test is slightly different than the other one that looks like it,
    //  because the bounds are adjacent to each other
    reg = IntegerRegion::Union(IntegerRange(5, 0), IntegerRange(10, 4));
    QVERIFY(reg.IsUniverseSet());

    /*   <----|   |---->
      <---|    |--->       */
    reg = IntegerRegion::Union(IntegerRange(5, 0), IntegerRange(1, -5));
    QVERIFY(reg.IsUniverseSet());
}

void RangeTest::testComplement()
{
    IntegerRegion reg;

    /*    |--|   */
    reg = IntegerRegion( IntegerRange(-5, 5) ).Complement();
    for(int i = -10; i <= -6; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -5; i <= 5; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 6; i <= 10; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /*    |--->   */
    IntegerRange r;
    r.SetLowerBound(0);
    reg = IntegerRegion(r).Complement();
    for(int i = -10; i <= -1; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 0; i <= 10; i++)
        QVERIFY(!reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));

    /*    <---|   */
    r = IntegerRange();
    r.SetUpperBound(0);
    reg = IntegerRegion(r).Complement();
    for(int i = -10; i <= 0; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 1; i <= 10; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /*    <--->   */
    r = r.Universe();
    reg = IntegerRegion(r).Complement();
    QVERIFY(reg.IsNull());
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));

    /*       */
    reg = reg.NullSet().Complement();
    QVERIFY(reg.IsUniverseSet());
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /*    |---|   |---|  becomes...   <---|   |--|   |--->   */
    reg = ~IntegerRegion::Union(IntegerRange(-10, -5), IntegerRange(5, 10));
    for(int i = -20; i <= -11; i++)
        QVERIFY(reg.Contains(i));
    for(int i = -10; i <= -5; i++)
        QVERIFY2(!reg.Contains(i), QString("%1").arg(i).toStdString().c_str());
    for(int i = -4; i <= 4; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 5; i <= 10; i++)
        QVERIFY2(!reg.Contains(i), QString("%1").arg(i).toStdString().c_str());
    for(int i = 11; i <= 20; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(reg.Contains(INFINITY));

    /*    |---|   |--->   becomes...   <---|   |----|    */
    r = IntegerRange();
    r.SetLowerBound(20);
    reg = IntegerRegion::Union(IntegerRange(0, 10), r).Complement();
    for(int i = -20; i <= -1; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 0; i <= 10; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 11; i <= 19; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 20; i <= 30; i++)
        QVERIFY(!reg.Contains(i));
    QVERIFY(reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));
}

void RangeTest::testIntercept()
{
    IntegerRegion reg;

    /*    |---|
                    |---|     */
    reg = IntegerRegion::Intersect(IntegerRange(-10, -5), IntegerRange(5, 10) );
    QVERIFY(reg.IsNull());

    /*    |---|
               |---|     */
    reg = IntegerRegion::Intersect(IntegerRange(0, 5), IntegerRange(6, 10) );
    QVERIFY(reg.IsNull());

    /*    |---|
             |---|     */
    reg = IntegerRegion::Intersect(IntegerRange(-10, 5), IntegerRange(-5, 10));
    for(int i = -20; i <= -6; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = -5; i <= 5; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 6; i <= 20; i++)
        QVERIFY(!reg.Contains(i));


    /*    |--->
             <---|     */
    IntegerRange r1, r2;
    r1.SetLowerBound(-5);
    r2.SetUpperBound(5);
    reg = IntegerRegion::Intersect(r1, r2);
    for(int i = -20; i <= -6; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = -5; i <= 5; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 6; i <= 20; i++)
        QVERIFY(!reg.Contains(i));

    /*          |--->
        <---|               */
    r1 = r2 = IntegerRange();
    r1.SetLowerBound(5);
    r2.SetUpperBound(-5);
    reg = IntegerRegion::Intersect(r1, r2);
    QVERIFY(reg.IsNull());

    /*  <----->
        <----->              */
    reg = IntegerRegion::Intersect(reg.UniverseSet(), reg.UniverseSet());
    QVERIFY(reg.IsUniverseSet());

    /*          */
    reg = IntegerRegion::Intersect(reg.NullSet(), reg.NullSet());
    QVERIFY(reg.IsNull());

    /*
                 |---|        */
    reg = IntegerRegion::Intersect(reg.NullSet(), IntegerRange(10, 20));
    QVERIFY(reg.IsNull());
}

void RangeTest::testSymmetricDifference()
{
    IntegerRegion reg;

    // First test regular subtraction

    /*   |----|
            |----|*/
    reg = IntegerRegion(IntegerRange(0, 10)) - IntegerRange(5, 20);
    for(int i = -20; i <= -1; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 0; i <= 4; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 5; i <= 20; i++)
        QVERIFY(!reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));



    // Then test symmetric difference (all those in A or B but not in both)

    /*   |----|
            |----|*/
    reg = IntegerRegion(IntegerRange(0, 10))
            .SymmetricDifference(IntegerRange(5, 20));
    for(int i = -20; i <= -1; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 0; i <= 4; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 5; i <= 10; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 11; i <= 20; i++)
        QVERIFY(reg.Contains(i));
    QVERIFY(!reg.Contains(-INFINITY));
    QVERIFY(!reg.Contains(INFINITY));
}



QTEST_APPLESS_MAIN(RangeTest);

#include "range_test.moc"

