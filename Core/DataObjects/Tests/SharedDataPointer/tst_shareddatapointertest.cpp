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
#include "Core/DataObjects/shareddatapointer.h"
#include "Core/DataObjects/gstring.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);


class SharedDataPointerTest : public QObject
{
    Q_OBJECT

public:
    SharedDataPointerTest();

private Q_SLOTS:
    void test_basic_function();
    void test_implicit_sharing();
};

SharedDataPointerTest::SharedDataPointerTest()
{
}


class shared_test_class : public SharedData
{
public:
    int Value;

    shared_test_class() :Value(0){}
    shared_test_class(const shared_test_class &o) :Value(o.Value){}
    shared_test_class &operator = (const shared_test_class &o){ Value = o.Value; return *this; }

    ~shared_test_class(){ qDebug() << "I'm Deleting!!!"; }

};


void SharedDataPointerTest::test_basic_function()
{
    SharedDataPointer<shared_test_class> ptr(new shared_test_class);
    QVERIFY(ptr->Value == 0);
    QVERIFY((*ptr).Value == 0);

    ptr->Value = 1;

    SharedDataPointer<shared_test_class> ptr2( ptr );
    QVERIFY(ptr2->Value == 1);

    ptr->Value = 2;
    QVERIFY(ptr2->Value == 2);
}

void SharedDataPointerTest::test_implicit_sharing()
{
    ImplicitlySharedDataPointer<shared_test_class> ptr( new shared_test_class );
    ImplicitlySharedDataPointer<shared_test_class> ptr2( ptr );
    QVERIFY(const_cast<const ImplicitlySharedDataPointer<shared_test_class> &>(ptr2)->Value == 0);

    ptr->Value = 3;
    QVERIFY(const_cast<const ImplicitlySharedDataPointer<shared_test_class> &>(ptr2)->Value == 0);
    QVERIFY(const_cast<const ImplicitlySharedDataPointer<shared_test_class> &>(ptr)->Value == 3);


    ImplicitlySharedDataPointer<shared_test_class> ptr3( ptr );
    QVERIFY(const_cast<const ImplicitlySharedDataPointer<shared_test_class> &>(ptr3)->Value == 3);

    ptr3->Value = 4;
    QVERIFY(const_cast<const ImplicitlySharedDataPointer<shared_test_class> &>(ptr3)->Value == 4);
    QVERIFY(const_cast<const ImplicitlySharedDataPointer<shared_test_class> &>(ptr2)->Value == 0);
    QVERIFY2(const_cast<const ImplicitlySharedDataPointer<shared_test_class> &>(ptr)->Value == 3,
             String::Format("%d", const_cast<const ImplicitlySharedDataPointer<shared_test_class> &>(ptr)->Value));
}

QTEST_APPLESS_MAIN(SharedDataPointerTest);

#include "tst_shareddatapointertest.moc"
