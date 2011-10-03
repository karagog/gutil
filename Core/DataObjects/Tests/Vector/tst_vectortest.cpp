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
#include "Core/DataObjects/vector.h"
#include <vector>
GUTIL_USING_CORE_NAMESPACE(DataObjects);

class SimpleVectorTest : public QObject
{
    Q_OBJECT

public:
    SimpleVectorTest();

private Q_SLOTS:
    void test_basic_function();
    void test_removal();
    void test_vector_of_vector();
};

SimpleVectorTest::SimpleVectorTest()
{
}

void SimpleVectorTest::test_basic_function()
{
    Vector<int> vec;
    qDebug(QString("An empty vector uses %1 bytes, and a simple vector uses %2.\n"
                   "A std::vector uses %3")
           .arg(sizeof(Vector<int>))
           .arg(sizeof(SimpleVector<int>))
           .arg(sizeof(std::vector<int>))
           .toAscii().constData());

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
    QVERIFY(cnt == vec.Size());

    vec.Remove(vec.begin());
    QVERIFY(vec.Size() == 99);
    QVERIFY(vec[0] == 1);
    QVERIFY(vec[1] == 2);
    QVERIFY(vec[98] == 99);

    vec.Insert(0, vec.begin());
    QVERIFY(vec.Size() == 100);
    QVERIFY(vec[0] == 0);
    QVERIFY(vec[1] == 1);
    QVERIFY(vec[99] == 99);

    vec.Clear();
    QVERIFY(vec.Size() == 0);

    vec.Insert(2, vec.end());
    vec.Insert(3, vec.end());
    vec.Insert(4, vec.end());
    vec.Insert(1, vec.begin());
    vec.Insert(0, vec.begin());
    for(int i(0); i < 5; i++)
        QVERIFY(vec[i] == i);
}

void SimpleVectorTest::test_removal()
{
    Vector<int> vec;
    vec.Insert(0, vec.end());
    vec.Insert(1, vec.end());
    vec.Insert(2, vec.end());
    vec.Insert(3, vec.end());
    vec.Insert(4, vec.end());
    QVERIFY(vec.Size() == 5);

    vec.Remove(vec.begin());
    QVERIFY(vec.Size() == 4);
    for(int i(1); i < 5; i++)
        QVERIFY(vec[i - 1] == i);

    vec.Remove(vec.begin() + 2);
    QVERIFY(vec.Size() == 3);
    QVERIFY(vec[0] == 1);
    QVERIFY(vec[1] == 2);
    QVERIFY(vec[2] == 4);
}

void SimpleVectorTest::test_vector_of_vector()
{
    const int square_width = 3;
    int row1[] = { 0, 1, 2 };
    int row2[] = { 3, 4, 5 };
    int row3[] = { 6, 7, 8 };
    Vector< Vector<int> > matrix( square_width );
    matrix.PushBack( Vector<int>(row1, square_width) );
    matrix.PushBack( Vector<int>(row2, square_width) );
    matrix.PushBack( Vector<int>(row3, square_width) );

    SimpleVector< SimpleVector<int> > simple_matrix( 3 );
    simple_matrix.Insert( matrix[0], simple_matrix.end() );
    simple_matrix.Insert( matrix[1], simple_matrix.end() );
    simple_matrix.Insert( matrix[2], simple_matrix.end() );

    SimpleVector< SimpleVector<int> > simple_matrix_copy( simple_matrix );

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

QTEST_APPLESS_MAIN(SimpleVectorTest);

#include "tst_vectortest.moc"
