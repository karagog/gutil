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
#include "binarysearchtree.h"
#include <iostream>
#include <vector>
GUTIL_USING_CORE_NAMESPACE(DataObjects);
using namespace std;

class BST_Test :
        public QObject
{
    Q_OBJECT

public:
    BST_Test();

private Q_SLOTS:

    void test_iterators();
    void test_basic_function();

    void test_compare();

    void test_insertions();
    void test_deletions();

    void test_pointers();
    void test_multiset();

    void test_load();


private:
    static void show_depth_first_tree(const BinarySearchTree<int> &);

    static void show_breadth_first_tree(const BinarySearchTree<int> &);
    static void show_breadth_first_pointerTree(const BinarySearchTree<int *> &);

    static QString get_bft_string(const BST_NodeIterator<int> &iter);
    static QString get_bft_pointer_string(const BST_NodeIterator<int *> &);

    // You can feed in your own manual breadth-first description to test if
    //  the tree looks like you expect
    static bool tree_matches(const BinarySearchTree<int> &, const QString &);
    static bool tree_matches(const BinarySearchTree<int *> &, const QString &);

    static int backwards_compare(const int &, const int &);

    static int pointer_compare(int* const &, int* const &);

    static void verify_tree(const BinarySearchTree<int> &, bool diagnostics = false);

    static vector<int> export_df_tree(const BinarySearchTree<int> &);

    static void test_number_of_items(int);
};

void BST_Test::verify_tree(const BinarySearchTree<int> &bst, bool diagnostics)
{
    int mycnt(0);
    int mem(-1);
    for(BinarySearchTree<int>::const_iterator iter(bst.begin()); iter; ++iter, mycnt++)
    {
        // Make sure all items are in order
        QVERIFY(mem < *iter);
        mem = *iter;
        if(diagnostics)
            qDebug() << mem;
    }
    QVERIFY2(mycnt == bst.Size(), QString("%1 != %2").arg(mycnt).arg(bst.Size()).toAscii());

    // Iterate backwards to make sure it works that way too
    mem = INT_MAX;
    mycnt = 0;
    for(BinarySearchTree<int>::const_iterator iter(--bst.end()); iter; --iter, mycnt++)
    {
        int tmp( *iter );
        // Make sure all items are in order
        QVERIFY(mem > tmp);
        mem = tmp;
    }
    QVERIFY2(mycnt == bst.Size(), QString("%1 != %2").arg(mycnt).arg(bst.Size()).toAscii());
}

BST_Test::BST_Test()
{
    qsrand(QDateTime::currentDateTime().time().msec());
}

void BST_Test::test_iterators()
{
    BinarySearchTree<int> bst;
    bst.Add(3);
    bst.Add(5);
    bst.Add(1);
    QVERIFY(bst.Size() == 3);
    QVERIFY(bst.min() == 1);
    QVERIFY(bst.max() == 5);

    verify_tree(bst);


    // Try iterating backwards
    int mem = INT_MAX;
    for(BinarySearchTree<int>::const_iterator iter(--bst.end());
        iter;
        --iter)
    {
        // Make sure all items are in order
        QVERIFY(mem > *iter);
        mem = *iter;
    }

    // Create a tree with a lot of items for the next tests
    bst.Clear();
    for(int i(0); i < 100; i++)
        bst.Add(i);

    //show_breadth_first_tree(bst);

    // Pick a point somewhere in the middle and iterate to the end
    mem = -1;
    for(BinarySearchTree<int>::const_iterator iter(bst.Search(35));
        iter;
        ++iter)
    {
        // Make sure all items are in order
        //qDebug() << *iter;
        QVERIFY(mem < *iter);
        mem = *iter;
    }

    // Pick a point somewhere in the middle and iterate to the beginning
    mem = INT_MAX;
    for(BinarySearchTree<int>::const_iterator iter(bst.Search(65));
        iter;
        --iter)
    {
        // Make sure all items are in order
        QVERIFY(mem > *iter);
        mem = *iter;
    }
}

void BST_Test::test_basic_function()
{
    BinarySearchTree<int> bst;
    for(int i(1); i < 10; i++)
    {
        bst.Add(i);
        //cout<<"Step "<<i<<": ";
        //show_breadth_first_tree(bst);
    }
    //show_depth_first_tree(bst);
    //show_breadth_first_tree(bst);

    // Verify that the numbers are sorted
    verify_tree(bst);

    bst.Clear();

    for(int i(9); i >= 1; i--)
        bst.Add(i);
    verify_tree(bst);

    bst.Clear();

    // Add random numbers
    for(int i(0); i < 20; i++)
    {
        int new_num(qrand() % 1000);
        while(bst.Contains(new_num)) new_num = qrand() % 100;
        bst.Add(new_num);
        //show_breadth_first_tree(bst);
    }
    verify_tree(bst);

    //show_depth_first_tree(bst);

    // Add numbers in reverse order
    bst.Clear();
    bst.Add(5);
    bst.Add(4);
    bst.Add(3);
    bst.Add(2);
    bst.Add(1);
    bst.Add(0);
    verify_tree(bst, true);
}

int BST_Test::backwards_compare(const int &lhs, const int &rhs)
{
    if(lhs < rhs)
        return 1;
    else if(rhs < lhs)
        return -1;
    return 0;
}

void BST_Test::test_compare()
{
    BinarySearchTree<int> backwards_tree(&backwards_compare);
    for(int i(1); i < 10; i++)
        backwards_tree.Add(i);

    //show_depth_first_tree(backwards_tree);
    //show_breadth_first_tree(backwards_tree);

    // Verify that it sorted the numbers backwards
    vector<int> df( export_df_tree(backwards_tree) );
    QVERIFY(df.size() == 9);
    for(int i(0); i < (int)df.size(); i++)
        QVERIFY(df.at(i) == 9 - i);
}

void BST_Test::show_depth_first_tree(const BinarySearchTree<int> &t)
{
    cout<<"DF: ";
    vector<int> sorted_values(export_df_tree(t));
    for(int i(0); i < (int)sorted_values.size(); i++)
        cout<<sorted_values[i]<<" ";
    cout<<endl;
}

void BST_Test::show_breadth_first_tree(const BinarySearchTree<int> &t)
{
    cout<<"BF: ";
    cout<<get_bft_string(BST_NodeIterator<int>(t)).toStdString();
    cout<<endl;
}

void BST_Test::show_breadth_first_pointerTree(const BinarySearchTree<int *> &t)
{
    cout<<"BF: ";
    cout<<get_bft_pointer_string(BST_NodeIterator<int *>(t)).toStdString();
    cout<<endl;
}

QString BST_Test::get_bft_string(const BST_NodeIterator<int> &i)
{
    QString ret;
    BST_NodeIterator<int> iter(i);

    ret.append(QString("(%1, ").arg(iter.Value()));
    if(iter.CanDescendLeft())
    {
        iter.DescendLeft();
        ret.append(get_bft_string(iter));
        iter.Ascend();
    }
    ret.append(", ");
    if(iter.CanDescendRight())
    {
        iter.DescendRight();
        ret.append(get_bft_string(iter));
        iter.Ascend();
    }
    ret.append(")");

    return ret;
}

QString BST_Test::get_bft_pointer_string(const BST_NodeIterator<int *> &i)
{
    QString ret;
    BST_NodeIterator<int *> iter(i);

    ret.append(QString("(%1, ").arg(*iter.Value()));
    if(iter.CanDescendLeft())
    {
        iter.DescendLeft();
        ret.append(get_bft_pointer_string(iter));
        iter.Ascend();
    }
    ret.append(", ");
    if(iter.CanDescendRight())
    {
        iter.DescendRight();
        ret.append(get_bft_pointer_string(iter));
        iter.Ascend();
    }
    ret.append(")");

    return ret;
}

bool BST_Test::tree_matches(const BinarySearchTree<int *> &t, const QString &s)
{
    bool ret = get_bft_pointer_string(BST_NodeIterator<int *>(t)) == s;
    if(!ret)
    {
        cout<< get_bft_pointer_string(BST_NodeIterator<int *>(t)).toStdString() <<"\tNot equal to:"<<endl<<s.toStdString()<<endl;
    }
    return ret;
}

bool BST_Test::tree_matches(const BinarySearchTree<int> &t, const QString &s)
{
    bool ret = get_bft_string(BST_NodeIterator<int>(t)) == s;
    if(!ret)
    {
        cout<< get_bft_string(BST_NodeIterator<int>(t)).toStdString() <<"\tNot equal to:"<<endl<<s.toStdString()<<endl;
    }
    return ret;
}

void BST_Test::test_insertions()
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
    verify_tree(bst);

    // Then LL (Rotate Right)
    bst.Add(2);
    bst.Add(1);
    QVERIFY(tree_matches(bst, "(10, (2, (1, , ), (5, , )), (20, (15, , ), (25, , )))"));
    verify_tree(bst);

    // Then RL
    bst.Clear();
    bst.Add(5);
    bst.Add(10);
    bst.Add(9);
    QVERIFY(tree_matches(bst, "(9, (5, , ), (10, , ))"));
    verify_tree(bst);

    // Then LR
    bst.Clear();
    bst.Add(5);
    bst.Add(3);
    bst.Add(4);
    QVERIFY(tree_matches(bst, "(4, (3, , ), (5, , ))"));
    verify_tree(bst);
}

void BST_Test::test_deletions()
{
    BinarySearchTree<int> bst;

    // Test root node deletion first
    bst.Add(1);
    bst.Add(2);
    bst.Add(3);
    bst.Remove(2);
    verify_tree(bst);
    QVERIFY(bst.Size() == 2);

    // Delete LChild of root
    bst.Clear();
    bst.Add(1);
    bst.Add(2);
    bst.Add(3);
    bst.Remove(1);
    verify_tree(bst);
    QVERIFY(bst.Size() == 2);

    // Delete RChild of root
    bst.Clear();
    bst.Add(1);
    bst.Add(2);
    bst.Add(3);
    bst.Remove(3);
    verify_tree(bst);
    QVERIFY(bst.Size() == 2);

    // Delete root, replacement has no child
    bst.Clear();
    bst.Add(1);
    bst.Add(2);
    bst.Add(4);
    bst.Add(3);
    bst.Remove(2);
    //show_breadth_first_tree(bst);
    verify_tree(bst);

    // Delete root, replacement has child, and replacement is child of root
    bst.Clear();
    bst.Add(1);
    bst.Add(2);
    bst.Add(3);
    bst.Add(4);
    bst.Remove(2);
    verify_tree(bst);

    // Delete root, replacement has child, and replacement is not child of root
    bst.Clear();
    bst.Add(1);
    bst.Add(2);
    bst.Add(10);
    bst.Add(11);
    bst.Add(0);
    bst.Add(8);
    bst.Add(9);
    //show_breadth_first_tree(bst);
    bst.Remove(2);
    //show_breadth_first_tree(bst);
    verify_tree(bst);

    // Delete root, replacement (left) has no child
    bst.Clear();
    bst.Add(0);
    bst.Add(2);
    bst.Add(3);
    bst.Add(1);
    bst.Remove(2);
    verify_tree(bst);

    // Delete root, replacement (left) has child, and replacement is child of root
    bst.Clear();
    bst.Add(1);
    bst.Add(2);
    bst.Add(3);
    bst.Add(0);
    bst.Remove(2);
    verify_tree(bst);

    // Delete root, replacement has child, and replacement is not child of root
    bst.Clear();
    bst.Add(2);
    bst.Add(6);
    bst.Add(7);
    bst.Add(8);
    bst.Add(0);
    bst.Add(5);
    bst.Add(4);
    //show_breadth_first_tree(bst);
    bst.Remove(6);
    //show_breadth_first_tree(bst);
    verify_tree(bst);


    bst.Clear();
    const int cnt(50);
    const int max(5000);
    QList<int> record;
    for(int i = 0; i < cnt; i++)
    {
        int tmp(qrand() % max);
        while(bst.Contains(tmp))
            tmp = qrand() % max;
        record.append(tmp);
        bst.Add(tmp);
    }
    verify_tree(bst);

    for(int i = 0; i < cnt; i++)
    {
        //show_breadth_first_tree(bst);
        bst.Remove(record[i]);
        verify_tree(bst);
        QVERIFY(bst.Size() == cnt - i - 1);
    }
}

int BST_Test::pointer_compare(int *const&lhs, int *const&rhs)
{
    if(*lhs < *rhs)
        return -1;
    else if(*rhs < *lhs)
        return 1;
    return 0;
}

void BST_Test::test_pointers()
{
    BinarySearchTree<int *> pointer_tree(&pointer_compare);
    int a(1), b(2), c(3);
    pointer_tree.Add(&a);
    pointer_tree.Add(&b);
    pointer_tree.Add(&c);

    //show_breadth_first_pointerTree(pointer_tree);
    QVERIFY(tree_matches(pointer_tree, "(2, (1, , ), (3, , ))"));

    // Update the data pointed to and see if the tree data was also updated.
    a = 500;
    QVERIFY(tree_matches(pointer_tree, "(2, (500, , ), (3, , ))"));
    //show_breadth_first_pointerTree(pointer_tree);
}

void BST_Test::test_multiset()
{
//    MultiSet<int> set;
//    set.Add(1);
//    set.Add(1);
//    QVERIFY((*set.Search(1)).Count() == 2);
}

void BST_Test::test_load()
{
//    test_number_of_items(100000);

//    cout<<"======================================================"<<endl;

//    test_number_of_items(500000);

//    cout<<"======================================================"<<endl;

//    test_number_of_items(1000000);

//    cout<<"======================================================"<<endl;

//    test_number_of_items(5000000);

//    cout<<"======================================================"<<endl;

//    test_number_of_items(10000000);

//    cout<<"======================================================"<<endl;

//    test_number_of_items(20000000);
}

void BST_Test::test_number_of_items(int num_items)
{
    BinarySearchTree<int> bst;
    QTime total;
    QTime insert;
    QTime iterate;
    QTime remove;

    cout<<QString("Testing with %1 items...").arg(num_items).toStdString()<<endl;
    total.start();
    insert.start();
    for(int i(0); i < num_items; i++)
        bst.Add(i);
    int insert_msecs = insert.elapsed();

    iterate.start();
    for(BinarySearchTree<int>::const_iterator iter(bst.begin()); iter; ++iter) ;
    int iterate_msecs = iterate.elapsed();

    remove.start();
    for(int i(0); i < num_items; i++) bst.Remove(i);
    int remove_msecs = remove.elapsed();
    int total_msecs = total.elapsed();

    cout<<"Finished testing "<<num_items<<" items."<<endl;
    cout<<"Insertion:  "<<insert_msecs<<" milliseconds"<<endl;
    cout<<"Iteration:  "<<iterate_msecs<<" milliseconds"<<endl;
    cout<<"Removal:  "<<remove_msecs<<" milliseconds"<<endl;
    cout<<"Total:  "<<total_msecs<<" milliseconds"<<endl;
}



vector<int> BST_Test::export_df_tree(const BinarySearchTree<int> &t)
{
    vector<int> ret;
    for(BinarySearchTree<int>::const_iterator iter(t.begin()); iter != t.end(); iter++)
        ret.push_back(*iter);
    return ret;
}


QTEST_APPLESS_MAIN(BST_Test);

#include "tst_bst_testtest.moc"
