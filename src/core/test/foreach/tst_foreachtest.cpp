/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "gutil_globals.h"
#include <vector>
#include <set>
#include <map>
#include <QtTest>
using namespace std;
USING_NAMESPACE_GUTIL;

class ForeachTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void test_simple_foreach();
    void test_constness();
    void test_forever();
};

void ForeachTest::test_simple_foreach()
{
    // Iterate through a vector
    int cnt = 0;
    vector<int> v{0, 1, 2, 3, 4};
    ForEach(v, [&](int i){
        bool res;
        [&]{ QVERIFY(res = (cnt == i)); }();
        ++cnt;
        return res;
    });

    // Iterate through a vector in reverse
    cnt = 4;
    gForEachReverse(v, [&](int i){
        bool res;
        [&]{ QVERIFY(res = (cnt == i)); }();
        --cnt;
        return res;
    });

    // Iterate through a set
    cnt = 0;
    set<int> s{0, 1, 2, 3, 4};
    ForEach(s, [&](int i){
        bool res;
        [&]{ QVERIFY(res = (cnt == i)); }();
        ++cnt;
        return res;
    });

    // Iterate through a map
    cnt = 0;
    map<int, int> m{{0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}};
    ForEach(m, [&](const pair<int, int> &i){
        bool res;
        [&]{ QVERIFY(res = (cnt == i.first && cnt == i.second)); }();
        ++cnt;
        return res;
    });
}

void ForeachTest::test_constness()
{
    int cnt = 0;
    vector<int> v{0, 1, 2, 3, 4};

    // We can't change anything because all we have is a const reference
    ForEach((const vector<int> &)v, [&](const int &i){
        bool res;
        [&]{ QVERIFY(res = (cnt == i)); }();
        ++cnt;
        return res;
    });

    // This should also work
    cnt = 0;
    ForEach(v, [&](const int &i){
        bool res;
        [&]{ QVERIFY(res = (cnt == i)); }();
        ++cnt;
        return res;
    });

    // This should cause a compiler fail because we can't cast
    //  the contained type as a non-const.
    //ForEach((const vector<int> &)v, [](int &){});

    // We CAN change elements in the non-const container
    cnt = 0;
    ForEach(v, [&](int &i){
        bool res;
        [&]{ QVERIFY(res = (cnt == i)); }();
        ++cnt;
        i = 100;
        return res;
    });

    // Make sure all items changed
    ForEach(v, [](int i){
        bool res;
        [&]{ QVERIFY(res = (i == 100)); }();
        return res;
    });
}

void ForeachTest::test_forever()
{
    int cnt = 0;
    gForever([&]{
        ++cnt;
        if(cnt == 10)
            return false;
        return true;
    });
    QVERIFY(cnt == 10);
}

QTEST_APPLESS_MAIN(ForeachTest)

#include "tst_foreachtest.moc"
