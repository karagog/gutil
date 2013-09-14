/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <QString>
#include <QtTest>
#include "gutil_slist.h"
#include "gutil_dlist.h"
#include "gutil_vector.h"
#include "gutil_list.h"
#include "gutil_Icollections.h"
#include "gutil_smartpointer.h"
USING_NAMESPACE_GUTIL1(DataObjects);

class MemoryLeakChecker
{
public:

    GUtil::Utils::SmartPointer<int> Data;

    explicit MemoryLeakChecker(int value): Data(new int(value)){}
    MemoryLeakChecker(const MemoryLeakChecker &o) :Data(new int(*o.Data)){}

    MemoryLeakChecker &operator = (const MemoryLeakChecker &o){ Data = new int(*o.Data); }

};



class Data_object_interfacesTest :
        public QObject
{
    Q_OBJECT

public:
    Data_object_interfacesTest();
    
private Q_SLOTS:

    void testStack();
    void testQueue();
    void testDeque();
    void testRandomAccessContainer();

private:

    void testStack(IStack<int> &);
    void testStack(IStack<MemoryLeakChecker> &);

    void testQueue(IQueue<int> &);
    void testQueue(IQueue<MemoryLeakChecker> &);

    void testRandomAccessContainer(IRandomAccessContainer<int> &);
    void testRandomAccessContainer(IRandomAccessContainer<MemoryLeakChecker> &);

    void testDeque(IDeque<int> &);
    void testDeque(IDeque<MemoryLeakChecker> &);

};

Data_object_interfacesTest::Data_object_interfacesTest()
{
}

void Data_object_interfacesTest::testStack()
{
    // Run the POD tests
    Vector<int, IStack<int> > vec;
    SList<int, IStack<int> > slist;

    testStack(vec);
    testStack(slist);


    // Now check a more complex type, that allocates memory and must have its destructor called.
    //  This will validate that the container correctly calls the destructor on all items where necessary.
    Vector<MemoryLeakChecker, IStack<MemoryLeakChecker> > m_vec;
    SList<MemoryLeakChecker, IStack<MemoryLeakChecker> > m_slist;

    testStack(m_vec);
    testStack(m_slist);
}

void Data_object_interfacesTest::testQueue()
{

}

void Data_object_interfacesTest::testDeque()
{

}

void Data_object_interfacesTest::testRandomAccessContainer()
{

}




void Data_object_interfacesTest::testStack(IStack<int> &stack)
{
    int tmp;

    stack.Clear();
    QVERIFY(0 == stack.Size());

    for(int i = 1; i <= 4; i++)
    {
        tmp = stack.Push(i);
        QVERIFY(i == stack.Size());
        QVERIFY(i == tmp);
        QVERIFY(i == stack.Top());
        QVERIFY(i == const_cast<IStack<int> const &>(stack).Top());
    }

    stack.Pop();
    QVERIFY(3 == stack.Size());
    QVERIFY(3 == stack.Top());
    QVERIFY(3 == const_cast<IStack<int> const &>(stack).Top());

    stack.Pop();
    QVERIFY(2 == stack.Size());
    QVERIFY(2 == stack.Top());
    QVERIFY(2 == const_cast<IStack<int> const &>(stack).Top());

    stack.Pop();
    QVERIFY(1 == stack.Size());
    QVERIFY(1 == stack.Top());
    QVERIFY(1 == const_cast<IStack<int> const &>(stack).Top());

    stack.Pop();
    QVERIFY(0 == stack.Size());


    for(int i = 1; i <= 10000; i++)
    {
        tmp = stack.Push(i);
        QVERIFY(i == stack.Size());
        QVERIFY(i == tmp);
        QVERIFY(i == stack.Top());
        QVERIFY(i == const_cast<IStack<int> const &>(stack).Top());
    }

    stack.Clear();
    QVERIFY(0 == stack.Size());
}

void Data_object_interfacesTest::testStack(IStack<MemoryLeakChecker> &stack)
{
    MemoryLeakChecker tmp(0);

    stack.Clear();
    QVERIFY(0 == stack.Size());

    for(int i = 1; i <= 4; i++)
    {
        tmp = stack.Push(MemoryLeakChecker(i));
        QVERIFY(i == stack.Size());
        QVERIFY(i == *tmp.Data);
        QVERIFY(i == *stack.Top().Data);
        QVERIFY(i == *const_cast<IStack<MemoryLeakChecker> const &>(stack).Top().Data);
    }

    stack.Pop();
    QVERIFY(3 == stack.Size());
    QVERIFY(3 == *stack.Top().Data);
    QVERIFY(3 == *const_cast<IStack<MemoryLeakChecker> const &>(stack).Top().Data);

    stack.Pop();
    QVERIFY(2 == stack.Size());
    QVERIFY(2 == *stack.Top().Data);
    QVERIFY(2 == *const_cast<IStack<MemoryLeakChecker> const &>(stack).Top().Data);

    stack.Pop();
    QVERIFY(1 == stack.Size());
    QVERIFY(1 == *stack.Top().Data);
    QVERIFY(1 == *const_cast<IStack<MemoryLeakChecker> const &>(stack).Top().Data);

    stack.Pop();
    QVERIFY(0 == stack.Size());


    for(int i = 1; i <= 10000; i++)
    {
        tmp = stack.Push(MemoryLeakChecker(i));
        QVERIFY(i == stack.Size());
        QVERIFY(i == *tmp.Data);
        QVERIFY(i == *stack.Top().Data);
        QVERIFY(i == *const_cast<IStack<MemoryLeakChecker> const &>(stack).Top().Data);
    }

    stack.Clear();
    QVERIFY(0 == stack.Size());
}

void Data_object_interfacesTest::testQueue(IQueue<int> &)
{

}

void Data_object_interfacesTest::testQueue(IQueue<MemoryLeakChecker> &)
{

}

void Data_object_interfacesTest::testRandomAccessContainer(IRandomAccessContainer<int> &)
{

}

void Data_object_interfacesTest::testRandomAccessContainer(IRandomAccessContainer<MemoryLeakChecker> &)
{

}

void Data_object_interfacesTest::testDeque(IDeque<int> &)
{

}

void Data_object_interfacesTest::testDeque(IDeque<MemoryLeakChecker> &)
{

}

QTEST_APPLESS_MAIN(Data_object_interfacesTest)

#include "tst_data_object_interfacestest.moc"
