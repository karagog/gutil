/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "DataObjects/collection.h"
#include "Logging/debuglogger.h"
#include <QtCore/QString>
#include <QtTest/QtTest>
using namespace GUtil::DataObjects;


class test_data
{
public:
    test_data(QString s) :add_flag(false), remove_flag(false), id(s) {}

    bool operator == (const test_data &o){ return id == o.id; }

    bool add_flag;
    bool remove_flag;
    QString id;
};

class test_collection_t : public Collection<test_data *>
{
protected:
    virtual void on_add(test_data **d) const
    {
        // Mark the data that we added it
        (*d)->add_flag = true;
    }

    virtual void on_remove(test_data **d) const
    {
        // Mark the data that we removed it
        (*d)->remove_flag = true;
    }
};





class ContainerTest : public QObject
{
    Q_OBJECT

public:
    ContainerTest();

private Q_SLOTS:
    void test_add_remove();
    void test_resize();
};

ContainerTest::ContainerTest()
{
}

void ContainerTest::test_add_remove()
{
    test_collection_t coll;
    test_data d("one");

    coll.Add(&d);
    QVERIFY(d.add_flag);

    coll.RemoveOne(&d);
    QVERIFY(d.remove_flag);


    // Now test the removeAll function:
    test_data one("1");
    test_data two("1");
    test_data three("1");
    coll.Add(&one);
    coll.Add(&one);
    coll.Add(&two);
    coll.Add(&three);
    QVERIFY(coll.Count() == 4);

    // This should remove all of the ones
    coll.RemoveAll(&one);

    QVERIFY(coll.Count() == 2);
    QVERIFY(one.remove_flag);
    QVERIFY(!two.remove_flag);
    QVERIFY(!three.remove_flag);
}

void ContainerTest::test_resize()
{
    test_collection_t coll;
    coll.Add(&test_data("hi"));
    coll.Add(&test_data("hi"));

    coll.Resize(1);
    QVERIFY(coll.Size() == 1);

    bool exception_hit = false;
    try
    {
        // This should cause a notimplemented exception because the base collection
        //  class doesn't force you to declare a default constructor for the contained object
        coll.Resize(10);
    }
    catch(Core::NotImplementedException &)
    {
        exception_hit = true;
    }
    QVERIFY(exception_hit);
}

QTEST_APPLESS_MAIN(ContainerTest);

#include "tst_containertest.moc"
