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
    QVERIFY(tr1.IsUniverseSet());
}


#define INFINITY 999999

void RangeTest::testUnions()
{
    IntegerRegion reg;

    // First test bounded unions
    /*  |---|
                |---|    */
    reg = IntegerRegion::Union(IntegerRange(0, 10), IntegerRange(15, 20));
    for(int i = 0; i <= 10; i++)
        QVERIFY(reg.Contains(i));
    for(int i = 11; i <= 14; i++)
        QVERIFY(!reg.Contains(i));
    for(int i = 15; i <= 20; i++)
        QVERIFY2(reg.Contains(i), QString("%1").arg(i).toStdString().c_str());
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

    /*        |---->
       |---|            */

    /*        |---->
            |---|       */

    /*        <----|
            |---|       */

    /* <----|
               |---|    */

    /* <----|
          |---|         */


    // Then test both ranges unbounded
}

void RangeTest::testComplement()
{

}

void RangeTest::testSymmetricDifference()
{

}

void RangeTest::testIntercept()
{

}



QTEST_APPLESS_MAIN(RangeTest);

#include "range_test.moc"

