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

#include <QtCore/QString>
#include <QtTest/QtTest>
#include "Core/DataObjects/smartpointer.h"
#include "Core/DataObjects/string.h"
USING_NAMESPACE_GUTIL1(DataObjects);


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


bool smart_test_class_did_destruct = false;

class smart_test_class
{
public:
    smart_test_class() :Value(0){}
    ~smart_test_class(){ smart_test_class_did_destruct = true; }
    int Value;
};


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
    QVERIFY(!smart_test_class_did_destruct);
    {
        SmartPointer<smart_test_class> smart_pointer( new smart_test_class );
        QVERIFY(smart_pointer->Value == 0);

        smart_pointer->Value++;
        QVERIFY((*smart_pointer).Value == 1);

        QVERIFY(smart_pointer);
        smart_pointer = NULL;
        QVERIFY(!smart_pointer);
    }
    QVERIFY(smart_test_class_did_destruct);

    SharedSmartPointer<shared_test_class> ptr(new shared_test_class);
    QVERIFY(ptr->Value == 0);
    QVERIFY((*ptr).Value == 0);

    ptr->Value = 1;

    SharedSmartPointer<shared_test_class> ptr2( ptr );
    QVERIFY(ptr2->Value == 1);

    ptr->Value = 2;
    QVERIFY(ptr2->Value == 2);
}

void SharedDataPointerTest::test_implicit_sharing()
{
    ImplicitlySharedSmartPointer<shared_test_class> ptr( new shared_test_class );
    ImplicitlySharedSmartPointer<shared_test_class> ptr2( ptr );
    QVERIFY(const_cast<const ImplicitlySharedSmartPointer<shared_test_class> &>(ptr2)->Value == 0);

    ptr->Value = 3;
    QVERIFY(const_cast<const ImplicitlySharedSmartPointer<shared_test_class> &>(ptr2)->Value == 0);
    QVERIFY(const_cast<const ImplicitlySharedSmartPointer<shared_test_class> &>(ptr)->Value == 3);


    ImplicitlySharedSmartPointer<shared_test_class> ptr3( ptr );
    QVERIFY(const_cast<const ImplicitlySharedSmartPointer<shared_test_class> &>(ptr3)->Value == 3);

    ptr3->Value = 4;
    QVERIFY(const_cast<const ImplicitlySharedSmartPointer<shared_test_class> &>(ptr3)->Value == 4);
    QVERIFY(const_cast<const ImplicitlySharedSmartPointer<shared_test_class> &>(ptr2)->Value == 0);
    QVERIFY2(const_cast<const ImplicitlySharedSmartPointer<shared_test_class> &>(ptr)->Value == 3,
             String::Format("%d", const_cast<const ImplicitlySharedSmartPointer<shared_test_class> &>(ptr)->Value));
}

QTEST_APPLESS_MAIN(SharedDataPointerTest);

#include "tst_shareddatapointertest.moc"
