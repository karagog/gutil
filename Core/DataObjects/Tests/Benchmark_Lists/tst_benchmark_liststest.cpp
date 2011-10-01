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
#include "Core/DataObjects/slist.h"
#include "Core/DataObjects/dlist.h"
#include "Core/DataObjects/list.h"
#include "Core/DataObjects/vector.h"
#include <iostream>
#include <stack>
#include <queue>
using namespace std;
GUTIL_USING_CORE_NAMESPACE(DataObjects);

class Benchmark_ListsTest :
        public QObject
{
    Q_OBJECT

public:
    Benchmark_ListsTest();

    /** A wrapper around an STL stack so we can compare ours with theirs. */
    template<class T>class STL_Stack :
            public stack<T>,
            public Stack<T>
    {
    public:
        void Push(const T &i){ stack<T>::push(i); }
        void Pop(){ stack<T>::pop(); }
        T &Top(){ return stack<T>::top(); }
        const T &Top() const{ return stack<T>::top(); }
        void FlushStack(){  }
        GUINT32 CountStackItems() const{ return stack<T>::size(); }
    };


private Q_SLOTS:

    void test_stacks();


private:

    void test_stacks(int num_items);
    void test_stack(Stack<int> *, int num_items);
    int stack_push(Stack<int> *, int num_items);
    int stack_top(Stack<int> *, int num_items);
    int stack_pop(Stack<int> *, int num_items);

    int queue_enqueue(Queue<int> *, int num_items);
    int queue_front(Queue<int> *, int num_items);
    int queue_dequeue(Queue<int> *, int num_items);

    int deque_enqueue(Deque<int> *, int num_items);
    int deque_front(Deque<int> *, int num_items);
    int deque_back(Deque<int> *, int num_items);
    int deque_dequeue(Deque<int> *, int num_items);

    int rac_insert(RandomAccessContainer<int> *, int num_items);
    int rac_touch_all_values(RandomAccessContainer<int> *, int num_items);
    int rac_remove(RandomAccessContainer<int> *, int num_items);

};


#define NUMBER_OF_ITEMS         1000000
#define NUMBER_OF_REPETITIONS   10



Benchmark_ListsTest::Benchmark_ListsTest()
{
}

void Benchmark_ListsTest::test_stacks()
{
    int num_items( NUMBER_OF_ITEMS );

    cout<<"Testing with "<< num_items <<" items..."<<endl;
    test_stacks(num_items);

    num_items <<= 1;

    cout<<"Testing with "<< num_items <<" items..."<<endl;
    test_stacks(num_items);

    num_items <<= 1;

    cout<<"Testing with "<< num_items <<" items..."<<endl;
    test_stacks(num_items);
}

void Benchmark_ListsTest::test_stacks(int num_items)
{
    SList<int> slist;
    DList<int> dlist;
    Vector<int> vector;
    List<int> list;
    STL_Stack<int> stl;

    cout<<"Stack as SList:"<<endl;
    test_stack(&slist, num_items);

    cout<<"Stack as DList:"<<endl;
    test_stack(&dlist, num_items);

    cout<<"Stack as a vector:"<<endl;
    test_stack(&vector, num_items);

    cout<<"Stack as a list:"<<endl;
    test_stack(&list, num_items);

    cout<<"STL Stack (implemented as deque):"<<endl;
    test_stack(&stl, num_items);
}



void Benchmark_ListsTest::test_stack(Stack<int> *s, int num_items)
{
    float push_avg(0);
    float top_avg(0);
    float pop_avg(0);
    cout<<endl;
    for(int i(0); i < NUMBER_OF_REPETITIONS; ++i)
    {
        //cout << ".";cout.flush();
        push_avg += stack_push(s, num_items);
        top_avg += stack_top(s, num_items);
        pop_avg += stack_pop(s, num_items);
    }

    cout<<endl;
    cout<<"\tPush:\t"<<push_avg / NUMBER_OF_REPETITIONS<<endl;
    cout<<"\tTop:\t"<<top_avg / NUMBER_OF_REPETITIONS<<endl;
    cout<<"\tPop:\t"<<pop_avg / NUMBER_OF_REPETITIONS<<endl;
    cout<<endl;
}






int Benchmark_ListsTest::stack_push(Stack<int> *s, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) s->Push(0);
    return t.elapsed();
}

int Benchmark_ListsTest::stack_top(Stack<int> *s, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) int tmp(s->Top());
    return t.elapsed();
}

int Benchmark_ListsTest::stack_pop(Stack<int> *s, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) s->Pop();
    return t.elapsed();
}

int Benchmark_ListsTest::queue_enqueue(Queue<int> *q, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) q->Enqueue(0);
    return t.elapsed();
}

int Benchmark_ListsTest::queue_front(Queue<int> *q, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) int t(q->Front());
    return t.elapsed();
}

int Benchmark_ListsTest::queue_dequeue(Queue<int> *q, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) q->Dequeue();
    return t.elapsed();
}









QTEST_APPLESS_MAIN(Benchmark_ListsTest);

#include "tst_benchmark_liststest.moc"
