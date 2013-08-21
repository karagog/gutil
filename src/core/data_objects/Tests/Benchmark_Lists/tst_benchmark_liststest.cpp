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
#include "gutil.h"
#include "gutil_slist.h"
#include "gutil_dlist.h"
#include "gutil_list.h"
#include "gutil_vector.h"
#include <iostream>
#include <stack>
#include <queue>
using namespace std;
USING_NAMESPACE_GUTIL1(DataObjects);

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

    void test_stacks_int();
    void test_stacks_large_class();


private:

    void test_stacks_int(int num_items);
    void test_stacks_large_class(int num_items);
    template<class T>void test_stack(Stack<T> *, int num_items);

    template<class T>int stack_push(Stack<T> *, int num_items);
    template<class T>int stack_top(Stack<T> *, int num_items);
    template<class T>int stack_pop(Stack<T> *, int num_items);

    template<class T>int queue_enqueue(Queue<T> *, int num_items);
    template<class T>int queue_front(Queue<T> *, int num_items);
    template<class T>int queue_dequeue(Queue<T> *, int num_items);

    template<class T>int deque_enqueue(Deque<T> *, int num_items);
    template<class T>int deque_front(Deque<T> *, int num_items);
    template<class T>int deque_back(Deque<T> *, int num_items);
    template<class T>int deque_dequeue(Deque<T> *, int num_items);

    template<class T>int rac_insert(RandomAccessContainer<T> *, int num_items);
    template<class T>int rac_touch_all_values(RandomAccessContainer<T> *, int num_items);
    template<class T>int rac_remove(RandomAccessContainer<T> *, int num_items);

};


#define NUMBER_OF_ITEMS         500000
#define NUMBER_OF_REPETITIONS   10



Benchmark_ListsTest::Benchmark_ListsTest()
{
}

void Benchmark_ListsTest::test_stacks_int()
{
    int num_items( NUMBER_OF_ITEMS );

    cout<<"Testing with "<< num_items <<" items..."<<endl;
    test_stacks_int(num_items);

    num_items <<= 1;

    cout<<"Testing with "<< num_items <<" items..."<<endl;
    test_stacks_int(num_items);

    num_items <<= 1;

    cout<<"Testing with "<< num_items <<" items..."<<endl;
    test_stacks_int(num_items);
}

void Benchmark_ListsTest::test_stacks_int(int num_items)
{
    SListStack<int> slist;
    DListStack<int> dlist;
    VectorStack<int> vector;
    ListStack<int> list;
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

void Benchmark_ListsTest::test_stacks_large_class()
{
    int num_items( NUMBER_OF_ITEMS );

    cout<<"Testing with "<< num_items <<" items..."<<endl;
    test_stacks_large_class(num_items);

    num_items <<= 1;

    cout<<"Testing with "<< num_items <<" items..."<<endl;
    test_stacks_large_class(num_items);

    num_items <<= 1;

    cout<<"Testing with "<< num_items <<" items..."<<endl;
    test_stacks_large_class(num_items);
}



template<class T>void Benchmark_ListsTest::test_stack(Stack<T> *s, int num_items)
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



class large_class
{
public:
    int one, two, three, four, five, six, seven, eight, nine, ten;
    int eleven, twelve, thirteen, fourteen, fifteen, sixteen, seventeen, eightteen, nineteen, twenty;
    int twentyone, twentytwo, twentythree, twentyfour, twentyfive, twentysix, twentyseven, twentyeight, twentynine, thirty;
    int thirtyone, thirtytwo, thirtythree, thirtyfour, thirtyfive, thirtysix, thirtyseven, thirtyeight, thirtynine, fourty;
};

void Benchmark_ListsTest::test_stacks_large_class(int num_items)
{
    SListStack<large_class> slist;
    DListStack<large_class> dlist;
    VectorStack<large_class> vector;
    ListStack<large_class> list;
    STL_Stack<large_class> stl;

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




template<class T>
int Benchmark_ListsTest::stack_push(Stack<T> *s, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) s->Push(T());
    return t.elapsed();
}

template<class T>
int Benchmark_ListsTest::stack_top(Stack<T> *s, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) T tmp(s->Top());
    return t.elapsed();
}

template<class T>
int Benchmark_ListsTest::stack_pop(Stack<T> *s, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) s->Pop();
    return t.elapsed();
}

template<class T>
int Benchmark_ListsTest::queue_enqueue(Queue<T> *q, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) q->Enqueue(T());
    return t.elapsed();
}

template<class T>
int Benchmark_ListsTest::queue_front(Queue<T> *q, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) T t(q->Front());
    return t.elapsed();
}

template<class T>int Benchmark_ListsTest::queue_dequeue(Queue<T> *q, int num_items)
{
    int i(0);
    QTime t; t.start();
    for(; i < num_items; ++i) q->Dequeue();
    return t.elapsed();
}









QTEST_APPLESS_MAIN(Benchmark_ListsTest);

#include "tst_benchmark_liststest.moc"
