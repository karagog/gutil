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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include "Custom/gdatetime.h"
GUTIL_USING_NAMESPACE(Custom);

class GDateTimeTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testBasics();
    void testUnions();

};

void GDateTimeTest::testBasics()
{
    TimeRegion reg;
    QVERIFY(reg.IsNull());
    QVERIFY(!reg.IsUniverseSet());

    TimeRange tr1;
    QVERIFY(tr1.IsUniverseSet());
}

void GDateTimeTest::testUnions()
{
    TimeRegion reg;

    // First test bounded unions
    /*  |---|
                |---|    */
    reg = TimeRegion::Union(TimeRange(QDate(2000, 1, 10), QDate(2000, 1, 13)),
                                       TimeRange(QDate(2000, 1, 15), QDate(2000, 1, 20)));
    QVERIFY(reg.Contains(QDate(2000, 1, 10)));
    QVERIFY(reg.Contains(QDate(2000, 1, 13)));
    QVERIFY(reg.Contains(QDate(2000, 1, 15)));
    QVERIFY(reg.Contains(QDate(2000, 1, 20)));
    QVERIFY(!reg.Contains(QDate(2000, 1, 9)));
    QVERIFY(!reg.Contains(QDate(2000, 1, 21)));

    /*  |---|
          |---|     */
    reg = TimeRegion::Union(TimeRange(QDate(2000, 1, 10), QDate(2000, 1, 15)),
                           TimeRange(QDate(2000, 1, 13), QDate(2000, 1, 20)));
    QVERIFY(reg.RangeCount() == 1);
    QVERIFY(reg.Contains(QDate(2000, 1, 10)));
    QVERIFY(reg.Contains(QDate(2000, 1, 20)));
    QVERIFY(reg.Contains(QDate(2000, 1, 14)));
    QVERIFY(!reg.Contains(QDate(2000, 1, 9)));
    QVERIFY(!reg.Contains(QDate(2000, 1, 21)));

    /*  |---|
         |-|       */
    reg = TimeRegion::Union(TimeRange(QDate(2000, 1, 10), QDate(2000, 1, 20)),
                           TimeRange(QDate(2000, 1, 13), QDate(2000, 1, 15)));
    QVERIFY(reg.RangeCount() == 1);
    QVERIFY(reg.Contains(QDate(2000, 1, 10)));
    QVERIFY(reg.Contains(QDate(2000, 1, 20)));
    QVERIFY(reg.Contains(QDate(2000, 1, 14)));
    QVERIFY(!reg.Contains(QDate(2000, 1, 9)));
    QVERIFY(!reg.Contains(QDate(2000, 1, 21)));

    /*  |---|
      |---|       */
    reg = TimeRegion::Union(TimeRange(QDate(2000, 1, 13), QDate(2000, 1, 20)),
                           TimeRange(QDate(2000, 1, 10), QDate(2000, 1, 15)));
    QVERIFY(reg.RangeCount() == 1);
    QVERIFY(reg.Contains(QDate(2000, 1, 10)));
    QVERIFY(reg.Contains(QDate(2000, 1, 20)));
    QVERIFY(reg.Contains(QDate(2000, 1, 14)));
    QVERIFY(!reg.Contains(QDate(2000, 1, 9)));
    QVERIFY(!reg.Contains(QDate(2000, 1, 21)));

    /*       |---|
      |---|       */
    reg = TimeRegion::Union(TimeRange(QDate(2000, 1, 15), QDate(2000, 1, 20)),
                           TimeRange(QDate(2000, 1, 10), QDate(2000, 1, 13)));
    QVERIFY(reg.Contains(QDate(2000, 1, 10)));
    QVERIFY(reg.Contains(QDate(2000, 1, 13)));
    QVERIFY(reg.Contains(QDate(2000, 1, 15)));
    QVERIFY(reg.Contains(QDate(2000, 1, 20)));
    QVERIFY(!reg.Contains(QDate(2000, 1, 9)));
    QVERIFY(!reg.Contains(QDate(2000, 1, 21)));


    // Then test one bounded range, one unbounded
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

QTEST_APPLESS_MAIN(GDateTimeTest);

#include "tst_gdatetimetest.moc"
