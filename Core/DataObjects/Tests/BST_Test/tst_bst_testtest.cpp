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

private:
    static void show_depth_first_tree(const BinarySearchTree<int> &);

    static void show_breadth_first_tree(const BinarySearchTree<int> &);
    static void show_btf(BST_NodeIterator<int> &iter);
};

BST_TestTest::BST_TestTest()
{
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

    bst.Clear();

    for(int i(9); i >= 1; i--)
        bst.Add(i);
    show_depth_first_tree(bst);
    show_breadth_first_tree(bst);
}

void BST_TestTest::show_depth_first_tree(const BinarySearchTree<int> &t)
{
    cout<<"DF: ";
    vector<int> sorted_values(t.ExportDepthFirst());
    for(int i(0); i < (int)sorted_values.size(); i++)
        cout<<sorted_values[i];
    cout<<endl;
}

void BST_TestTest::show_breadth_first_tree(const BinarySearchTree<int> &t)
{
    cout<<"BF: ";
    BST_NodeIterator<int> iter(t.Root());
    show_btf(iter);
    cout<<endl;
}

void BST_TestTest::show_btf(BST_NodeIterator<int> &iter)
{
    cout<<"("<<iter.Value()<<", ";
    if(iter.CanDescendLeft())
    {
        iter.DescendLeft();
        show_btf(iter);
        iter.Ascend();
    }
    cout<<", ";
    if(iter.CanDescendRight())
    {
        iter.DescendRight();
        show_btf(iter);
        iter.Ascend();
    }
    cout<<")";
}

QTEST_APPLESS_MAIN(BST_TestTest);

#include "tst_bst_testtest.moc"
