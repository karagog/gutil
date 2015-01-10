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
#include <gutil/vector.h>
#include <vector>
USING_NAMESPACE_GUTIL;

class VectorTest : public QObject
{
    Q_OBJECT

public:
    VectorTest();

private Q_SLOTS:
    void test_basic_function();
    void test_pushfront();
    void test_removal();
    void test_vector_of_vector();
    void test_non_movable_class();
    void test_sorting();

    void test_interfaces();


private:

    void _test_irandomaccess(IRandomAccessContainer<int> *);
    void _test_iqueue(IQueue<int> *);
    void _test_ideque(IDeque<int> *);
    void _test_istack(IStack<int> *);

};


/** Note: You should be able to catch failures to deconstruct with valgrind
*/
class nonmovable
{
public:
    inline nonmovable() :this_ptr(this), my_int(new int){}
    inline nonmovable(const nonmovable &) :this_ptr(this), my_int(new int){}
    inline nonmovable &operator = (const nonmovable &){
        delete my_int;
        my_int = new int;
        this_ptr = this;
        return *this;
    }
    inline ~nonmovable(){ delete my_int; }

    nonmovable *this_ptr;
    int *my_int;
};




VectorTest::VectorTest()
{
}

void VectorTest::test_basic_function()
{
    Vector<int> vec;
    qDebug(QString("An empty vector uses %1 bytes, and a std::vector uses %2.")
           .arg(sizeof(Vector<int>))
           .arg(sizeof(std::vector<int>))
           .toLatin1().constData());

    qDebug(QString("An empty vector with a random access interface uses %1 bytes.")
           .arg(sizeof(Vector<int, true, IRandomAccessContainer<int> >))
           .toLatin1().constData());

    for(int i(0); i < 100; ++i)
    {
        vec.Insert(i, vec.end());
//        qDebug(QString("After item %1, memory usage is %2 bytes\n")
//               .arg(i).arg(vec.ReportMemoryUsage()).toAscii());
    }

    int cnt(0);
    for(Vector<int>::const_iterator iter(vec.begin()); iter; ++iter)
    {
        QVERIFY(*iter == cnt);
        ++cnt;
    }
    QVERIFY(cnt == vec.Length());

    vec.Remove(vec.begin());
    QVERIFY(vec.Length() == 99);
    QVERIFY(vec[0] == 1);
    QVERIFY(vec[1] == 2);
    QVERIFY(vec[98] == 99);

    vec.Insert(0, vec.begin());
    QVERIFY(vec.Length() == 100);
    QVERIFY(vec[0] == 0);
    QVERIFY(vec[1] == 1);
    QVERIFY(vec[99] == 99);

    vec.Clear();
    QVERIFY(vec.Length() == 0);

    vec.Insert(2, vec.end());
    vec.Insert(3, vec.end());
    vec.Insert(4, vec.end());
    vec.Insert(1, vec.begin());
    vec.Insert(0, vec.begin());
    for(int i(0); i < 5; i++)
        QVERIFY(vec[i] == i);
}

void VectorTest::test_pushfront()
{
    static const int count = 1000;

    Vector<int> vec;
    for(int i = count; i >= 1; --i)
        vec.PushFront(i);

    QVERIFY(vec.Length() == count);

    for(int *b = vec.Data(), *e = b + count, i = 1;
        b != e;
        ++b, ++i)
    {
        QVERIFY(*b == i);
    }
}

void VectorTest::test_removal()
{
    Vector<int> vec;
    vec.Insert(0, vec.end());
    vec.Insert(1, vec.end());
    vec.Insert(2, vec.end());
    vec.Insert(3, vec.end());
    vec.Insert(4, vec.end());
    QVERIFY(vec.Length() == 5);

    vec.Remove(vec.begin());
    QVERIFY(vec.Length() == 4);
    for(int i(1); i < 5; i++)
        QVERIFY(vec[i - 1] == i);

    vec.Remove(vec.begin() + 2);
    QVERIFY(vec.Length() == 3);
    QVERIFY(vec[0] == 1);
    QVERIFY(vec[1] == 2);
    QVERIFY(vec[2] == 4);


    // Remove a range of items
    vec.Clear();
    vec.Insert(1);
    vec.Insert(2);
    vec.Insert(3);
    vec.Insert(4);
    vec.Insert(5);
    vec.Insert(6);
    vec.Insert(7);
    vec.Insert(8);
    vec.Insert(9);
    vec.Insert(10);
    vec.RemoveAt(1, 8);
    QVERIFY(vec.Length() == 2);
    QVERIFY(vec[0] == 1);
    QVERIFY(vec[1] == 10);


    // Remove a range of items that are not binary-movable
    Vector<nonmovable> nmvec(10);
    nmvec.Resize(10);
    nmvec.RemoveAt(2, 2);
    QVERIFY(nmvec.Length() == 8);

    nmvec.Clear();
    nmvec.Resize(10);
    nmvec.RemoveAt(1, 8);
    QVERIFY(nmvec.Length() == 2);
}

void VectorTest::test_vector_of_vector()
{
    const int square_width = 3;
    int row1[] = { 0, 1, 2 };
    int row2[] = { 3, 4, 5 };
    int row3[] = { 6, 7, 8 };
    Vector< Vector<int> > matrix( square_width );
    matrix.Insert(Vector<int>(row1, square_width), 0 );
    matrix.Insert(Vector<int>(row3, square_width), 1 );
    matrix.Insert(Vector<int>(row2, square_width), 1 );

    Vector< Vector<int> > simple_matrix( 3 );
    simple_matrix.Insert( matrix[0], simple_matrix.end() );
    simple_matrix.Insert( matrix[1], simple_matrix.end() );
    simple_matrix.Insert( matrix[2], simple_matrix.end() );

    Vector< Vector<int> > simple_matrix_copy( simple_matrix );

    for(int i(0); i < square_width; ++i)
    {
        for(int j(0); j < square_width; j++)
        {
            QVERIFY(matrix[i][j] == square_width*i + j);
            QVERIFY(simple_matrix[i][j] == square_width*i + j);
            QVERIFY(simple_matrix_copy[i][j] == square_width*i + j);
        }
    }

//    qDebug(QString("The %1-sized vector-of-vector<int> consumes %2 bytes of memory")
//           .arg(square_width).arg(matrix.ReportMemoryUsage()).toAscii());
}

void VectorTest::test_non_movable_class()
{
    Vector<nonmovable> vec;
    for(int i(0); i < 100; ++i)
        vec.Insert(nonmovable());

    int cnt(0);
    for(Vector<nonmovable>::const_iterator iter(vec.begin()); iter; ++iter)
    {
        const nonmovable &probe1( *iter );
        const nonmovable *probe2( vec.Data() + cnt );
        QVERIFY(probe1.this_ptr == &vec[cnt]);
        ++cnt;
    }
    QVERIFY(cnt == vec.Length());

    vec.Remove(vec.begin());
    QVERIFY(vec.Length() == 99);
    QVERIFY(vec[0].this_ptr == &vec[0]);
    QVERIFY(vec[1].this_ptr == &vec[1]);
    QVERIFY(vec[98].this_ptr == &vec[98]);

    vec.Insert(nonmovable(), vec.begin());
    QVERIFY(vec.Length() == 100);
    QVERIFY(vec[0].this_ptr == &vec[0]);
    QVERIFY(vec[1].this_ptr == &vec[1]);
    QVERIFY(vec[99].this_ptr == &vec[99]);

    vec.Clear();
    QVERIFY(vec.Length() == 0);

    vec.Insert(nonmovable(), vec.end());
    vec.Insert(nonmovable(), vec.end());
    vec.Insert(nonmovable(), vec.end());
    vec.Insert(nonmovable(), vec.begin());
    vec.Insert(nonmovable(), vec.begin());
    for(int i(0); i < 5; i++)
        QVERIFY(vec[i].this_ptr == &vec[i]);
}

void VectorTest::test_sorting()
{
    Vector<int> v;
    v.Insert(10);
    v.Insert(9);
    v.Insert(8);
    v.Insert(7);
    v.Insert(6);
    v.Insert(5);
    v.Insert(4);
    v.Insert(3);
    v.Insert(2);
    v.Insert(1);
    v.Insert(0);
    v.Sort(true, GUtil::IComparer<int>(), GUtil::MergeSort);

//    for(int i = 0; i < 11; ++i)
//        qDebug() << v[i];

    for(int i = 0; i < 11; ++i)
        QVERIFY2(v[i] == i, QString("%1 != %2").arg(v[i]).arg(i).toUtf8());


    v.Sort(false);
    for(int i = 10; i >= 0; --i)
        QVERIFY2(v[10 - i] == i, QString("%1 != %2").arg(v[10 - i]).arg(i).toUtf8());



    v.Clear();

    const int cnt(10000);
    for(int i = 0; i < cnt; ++i)
        v.Insert(qrand());

    v.Sort();
//    for(int i = 0; i < cnt; ++i)
//        qDebug() << v[i];

    QVERIFY(v.Length() == cnt);
    int mem(INT_MIN);
    for(int i = 0; i < cnt; ++i)
    {
        QVERIFY2(mem <= v[i], QString("%1 > %2").arg(mem).arg(v[i]).toUtf8());
        mem = v[i];
    }
}

void VectorTest::test_interfaces()
{
    Vector<int, true, IRandomAccessContainer<int> > vec_randomAccess;

    _test_irandomaccess(&vec_randomAccess);
}

void VectorTest::_test_irandomaccess(IRandomAccessContainer<int> *rac)
{
    rac->Clear();
    QVERIFY(0 == rac->Size());

    rac->Insert(2, 0);
    QVERIFY(1 == rac->Size());
    QVERIFY(2 == rac->At(0));

    rac->Insert(0, 0);
    QVERIFY(2 == rac->Size());
    QVERIFY(0 == rac->At(0));
    QVERIFY(2 == rac->At(1));

    rac->Insert(1, 1);
    QVERIFY(3 == rac->Size());
    QVERIFY(0 == rac->At(0));
    QVERIFY(1 == rac->At(1));
    QVERIFY(2 == rac->At(2));

    rac->Insert(3, 3);
    QVERIFY(4 == rac->Size());
    QVERIFY(0 == rac->At(0));
    QVERIFY(1 == rac->At(1));
    QVERIFY(2 == rac->At(2));
    QVERIFY(3 == rac->At(3));

    rac->Remove(0);
    QVERIFY(3 == rac->Size());
    QVERIFY(1 == rac->At(0));
    QVERIFY(2 == rac->At(1));
    QVERIFY(3 == rac->At(2));
}


QTEST_APPLESS_MAIN(VectorTest);

#include "tst_vectortest.moc"
