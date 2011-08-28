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
#include "BinarySearchTree.h"
#include <iostream>
GUTIL_USING_CORE_NAMESPACE(DataObjects);
using namespace std;

class BST_TestTest :
        public QObject
{
    Q_OBJECT

public:
    BST_TestTest();

private Q_SLOTS:
    void test_basic_function();

    void test_compare();

    void test_insertions();
    void test_deletions();


private:
    static void show_depth_first_tree(const BinarySearchTree<int> &);

    static void show_breadth_first_tree(const BinarySearchTree<int> &);
    static QString get_btf_string(const BST_NodeIterator<int> &iter);

    // You can feed in your own manual breadth-first description to test if
    //  the tree looks like you expect
    static bool tree_matches(const BinarySearchTree<int> &, const QString &);

    static int backwards_compare(const int &, const int &);
};

BST_TestTest::BST_TestTest()
{
    qsrand(QDateTime::currentDateTime().time().msec());
}

void BST_TestTest::test_basic_function()
{
    BinarySearchTree<int> bst;
    for(int i(1); i < 10; i++)
    {
        bst.Add(i);
        //cout<<"Step "<<i<<": ";
        //show_breadth_first_tree(bst);
    }
    show_depth_first_tree(bst);
    show_breadth_first_tree(bst);

    // Verify that the numbers are sorted
    vector<int> df( bst.ExportDepthFirst() );
    QVERIFY(df.size() == 9);
    for(int i(0); i < df.size(); i++)
        QVERIFY(df.at(i) == i + 1);

    bst.Clear();

    for(int i(9); i >= 1; i--)
        bst.Add(i);
    show_depth_first_tree(bst);
    show_breadth_first_tree(bst);

    // Verify that the numbers are sorted
    df = bst.ExportDepthFirst();
    QVERIFY(df.size() == 9);
    for(int i(0); i < df.size(); i++)
        QVERIFY(df.at(i) == i + 1);

    bst.Clear();

    // Add random numbers
    for(int i(0); i < 20; i++)
    {
        int new_num(qrand() % 1000);
        while(bst.HasItem(new_num)) new_num = qrand() % 100;
        bst.Add(new_num);
    }

    show_depth_first_tree(bst);
}

int BST_TestTest::backwards_compare(const int &lhs, const int &rhs)
{
    if(lhs < rhs)
        return 1;
    else if(rhs < lhs)
        return -1;
    return 0;
}

void BST_TestTest::test_compare()
{
    BinarySearchTree<int> backwards_tree(&BST_TestTest::backwards_compare);
    for(int i(1); i < 10; i++)
        backwards_tree.Add(i);

    show_depth_first_tree(backwards_tree);
    show_breadth_first_tree(backwards_tree);

    // Verify that it sorted the numbers backwards
    vector<int> df( backwards_tree.ExportDepthFirst() );
    QVERIFY(df.size() == 9);
    for(int i(0); i < df.size(); i++)
        QVERIFY(df.at(i) == 9 - i);
}

void BST_TestTest::show_depth_first_tree(const BinarySearchTree<int> &t)
{
    cout<<"DF: ";
    vector<int> sorted_values(t.ExportDepthFirst());
    for(int i(0); i < (int)sorted_values.size(); i++)
        cout<<sorted_values[i]<<" ";
    cout<<endl;
}

void BST_TestTest::show_breadth_first_tree(const BinarySearchTree<int> &t)
{
    cout<<"BF: ";
    cout<<get_btf_string(t.Root()).toStdString();
    cout<<endl;
}

QString BST_TestTest::get_btf_string(const BST_NodeIterator<int> &i)
{
    QString ret;
    BST_NodeIterator<int> iter(i);

    ret.append(QString("(%1, ").arg(iter.Value()));
    if(iter.CanDescendLeft())
    {
        iter.DescendLeft();
        ret.append(get_btf_string(iter));
        iter.Ascend();
    }
    ret.append(", ");
    if(iter.CanDescendRight())
    {
        iter.DescendRight();
        ret.append(get_btf_string(iter));
        iter.Ascend();
    }
    ret.append(")");

    return ret;
}

bool BST_TestTest::tree_matches(const BinarySearchTree<int> &t, const QString &s)
{
    bool ret = get_btf_string(t.Root()) == s;
    if(!ret)
    {
        cout<< get_btf_string(t.Root()).toStdString() <<"\tNot equal to:"<<endl<<s.toStdString()<<endl;
    }
    return ret;
}

void BST_TestTest::test_insertions()
{
    // Test the 4 cases of rebalancing:  LL, RR, LR, RL
    BinarySearchTree<int> bst;

    // This tests the RR condition twice (Rotate left)
    bst.Add(5);
    bst.Add(10);
    bst.Add(15);
    bst.Add(20);
    bst.Add(25);
    QVERIFY(tree_matches(bst, "(10, (5, , ), (20, (15, , ), (25, , )))"));

    // Then LL (Rotate Right)
    bst.Add(2);
    bst.Add(1);
    QVERIFY(tree_matches(bst, "(10, (2, (1, , ), (5, , )), (20, (15, , ), (25, , )))"));

    // Then RL
    bst.Clear();
    bst.Add(5);
    bst.Add(10);
    bst.Add(9);
    QVERIFY(tree_matches(bst, "(9, (5, , ), (10, , ))"));

    // Then LR
    bst.Clear();
    bst.Add(5);
    bst.Add(3);
    bst.Add(4);
    QVERIFY(tree_matches(bst, "(4, (3, , ), (5, , ))"));
}

void BST_TestTest::test_deletions()
{

}

QTEST_APPLESS_MAIN(BST_TestTest);

#include "tst_bst_testtest.moc"
