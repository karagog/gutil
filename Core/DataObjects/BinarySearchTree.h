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

#include "private/bst_node.h"
#include "Core/exception.h"
#include <vector>
#include <string>
#include <cstdlib>
#include <cassert>
using namespace std;

#ifndef GUTIL_BST_H
#define GUTIL_BST_H

GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** A class for iterating manually through the binary search tree.

    It's useful for debugging, but not much else.  You shouldn't normally
    have to use this class.  Because it's a template, if you don't use it
    it won't build into your application.
*/
template<class T> class BST_NodeIterator
{
public:
    BST_NodeIterator(bst_node *n)
        :m_node(n)
    {}

    /** The data held in this node. */
    const T &Value() const{
        return *((T*)m_node->Data);
    }

    /** Returns whether a left child exists. */
    bool CanDescendLeft() const{
        return m_node && m_node->LChild;
    }
    /** Returns whether a right child exists. */
    bool CanDescendRight() const{
        return m_node && m_node->RChild;
    }
    /** Returns whether you can move the iterator to the current's parent. */
    bool CanAscend() const{
        return m_node && m_node->Parent;
    }

    /** Advances the iterator into the left child. */
    bool DescendLeft(){
        if(CanDescendLeft()){
            m_node = m_node->LChild;
            return true;
        }
        return false;
    }
    /** Advances the iterator into the right child. */
    bool DescendRight(){
        if(CanDescendRight()){
            m_node = m_node->RChild;
            return true;
        }
        return false;
    }
    /** Advances the iterator to the current's parent */
    bool Ascend(){
        if(CanAscend()){
            m_node = m_node->Parent;
            return true;
        }
        return false;
    }

private:
    bst_node *m_node;
};



/** Implements an AVL-balanced binary search tree.

    BST's have the advantage of storing the data so it is always sorted,
    and can be looked up in O(log(n)) time.  Insertions, removals and lookups
    all are O(log).  Use this class to maintain a fast index of any kind of data.

    Implement your own compare function to control how the data is indexed.
*/
template<class T>class BinarySearchTree
{
public:

    /** Creates an empty BST with the default compare function (less-than operator). */
    BinarySearchTree();

    /** Creates an empty BST which uses your own custom compare function.

        \param compare A function to compare two instances of the specified type.
        It should return -1 if one is less than two, 1 if one is greater than 2,
        and 0 if they are equal.
    */
    BinarySearchTree(int (*compare)(const T &one,const T &two));

    /** Deletes the BST and cleans up all memory. */
    ~BinarySearchTree();


    /** Makes a copy of object and puts in the tree.

        If already exists in the tree an exception is thrown.
    */
    void Add(const T &);

    /** Removes the object from the tree.  Does nothing if object not in the tree. */
    void Remove(const T &);

    /** Removes all values from the BST. */
    void Clear();

    /** Returns whether the item is in the BST. */
    bool HasItem(const T &);

    /** Does a lookup on the given object and returns a const reference to its version of it.
        Throws an exception if not found.
    */
    const T &Search(const T &obj);


    BST_NodeIterator<T> Root() const{
        return BST_NodeIterator<T>(root);
    }

    /** Exports a depth-first (sorted) version of the contents of the tree */
    vector<T> ExportDepthFirst() const;


private:
    bst_node *root;
    int (*cmp)(const T &lhs, const T &rhs);

    /** The default compare function is the less-than operator. */
    static int default_compare(const T&, const T&);

    static vector<T> export_depth_first(bst_node *);
    static void cleanup_memory(bst_node *);

    // After the root node potentially moved, use this function to find the new root.
    void _update_root_node();

};


template<class T>int BinarySearchTree<T>::default_compare(const T &lhs, const T &rhs)
{
    if(lhs < rhs)
        return -1;
    else if(rhs < lhs)
        return 1;
    else
        return 0;
}


template<class T>void BinarySearchTree<T>::Add(const T &object)
{
    if(!root)
    {
        // If the root is null, then this is the first item in the tree (easy case)
        root = new bst_node;
        root->Data = (void *)new T(object);
        return;
    }

    // Find the place to insert, has to be a leaf node
    bst_node *cur( root );
    bst_node *next_cur( root );
    bst_node::SideEnum insertion_side(bst_node::LeftSide);
    while(next_cur)
    {
        cur = next_cur;
        int cmp_res( cmp(object, *((T*)cur->Data)) );

        if(cmp_res < 0)
        {
            next_cur = cur->LChild;
            insertion_side = bst_node::LeftSide;
        }
        else if(cmp_res > 0)
        {
            next_cur = cur->RChild;
            insertion_side = bst_node::RightSide;
        }
        else
        {
            // There's an insertion collision
            THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                       "Object already exists in BST.");
        }
    }

    bst_node *new_node( new bst_node );
    new_node->Data = (void *)new T(object);

    bst_node::Insert(cur, new_node, insertion_side);
    _update_root_node();
}

template<class T>bool BinarySearchTree<T>::HasItem(const T &object)
{
    bst_node *cur( root );
    while(cur)
    {
        int cmp_res( cmp(object, *((T*)cur->Data)) );

        if(cmp_res < 0)
            cur = cur->LChild;
        else if(cmp_res > 0)
            cur = cur->RChild;
        else
            return true;
    }
    return false;
}

template<class T>const T &BinarySearchTree<T>::Search(const T & object)
{
    bst_node *cur( root );
    bool found(false);

    while(!found && cur)
    {
        int cmp_res( cmp(object, *((T*)cur->Data)) );

        if(cmp_res < 0)
            cur = cur->LChild;
        else if(cmp_res > 0)
            cur = cur->RChild;
        else
            found = true;
    }

    if(found)
        return *((T*)cur->Data);
    else
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "Object not found in BST.");
}

template<class T>void BinarySearchTree<T>::Remove(const T &object)
{
    bst_node *cur( root );
    bool found(false);

    while(!found && cur)
    {
        int cmp_res( cmp(object, *((T*)cur->Data)) );

        if(cmp_res < 0)
            cur = cur->LChild;
        else if(cmp_res > 0)
            cur = cur->RChild;
        else
            found = true;
    }


    if(found)
    {
        // Remove it.  The deletion algorithm goes as follows:
        //  I need to find a replacement from the leaves of the tree to
        //  come take my place, then I will rebalance from the bottom of the tree.
        //  I will find a replacement as the left-most item on my right side, or my
        //  right-most item on my left side, whichever one is taller.

        bst_node::SideEnum side;
        bst_node *tmp_node(0);
        if(cur->HeightDifference() > 0)
        {
            tmp_node = cur->LChild;
            side = bst_node::LeftSide;
        }
        else
        {
            tmp_node = cur->RChild;
            side = bst_node::RightSide;
        }

        bst_node *tmp(tmp_node);
        while(tmp)
        {
            tmp_node = tmp;
            switch(side)
            {
            case bst_node::RightSide:
                tmp = tmp_node->LChild;
                break;
            case bst_node::LeftSide:
                tmp = tmp_node->RChild;
                break;
            }
        }


        if(root == cur)
        {
            // We have to pre-emptively adjust our root node, because we will be unable to
            //  find the new root once it's deleted.
            if(root->RChild)
                root = root->RChild;
            else if(root->LChild)
                root = root->LChild;
            else
                root = 0;
        }

        delete ((T *)cur->Data);
        bst_node::Delete(cur, tmp_node);

        _update_root_node();
    }
}

template<class T> vector<T> BinarySearchTree<T>::ExportDepthFirst() const
{
    return export_depth_first(root);
}

template<class T> vector<T> BinarySearchTree<T>::export_depth_first(bst_node *p)
{
    vector<T> ret;
    if(p)
    {
        ret = export_depth_first(p->LChild);
        ret.push_back(*((T *)p->Data));

        vector<T> rchildren( export_depth_first(p->RChild) );
        ret.reserve(ret.size() + rchildren.size());
        for(unsigned int i = 0; i < rchildren.size(); i++)
            ret.push_back(rchildren[i]);
    }
    return ret;
}

template<class T>BinarySearchTree<T>::BinarySearchTree()
    :root(0)
{
    cmp = &BinarySearchTree<T>::default_compare;
}

template<class T>BinarySearchTree<T>::BinarySearchTree(int (*compare)(const T &,const T &))
    :root(0)
{
    cmp = compare;
}

template<class T>BinarySearchTree<T>::~BinarySearchTree()
{
    Clear();
}

template<class T>void BinarySearchTree<T>::Clear()
{
    if(root)
    {
        // Need to iterate through the nodes and delete all the memory!  The bst_node
        //  can't do it because it stores the data as void *
        cleanup_memory(root);
        delete root;
        root = 0;
    }
}

template<class T>void BinarySearchTree<T>::cleanup_memory(bst_node *n)
{
    if(n)
    {
        cleanup_memory(n->LChild);
        cleanup_memory(n->RChild);
        delete ((T *)n->Data);
    }
}

template<class T>void BinarySearchTree<T>::_update_root_node()
{
    bst_node *n(root);
    while((n = n->Parent))
        root = n;
}


GUTIL_END_CORE_NAMESPACE

#endif //GUTIL_BST_H
