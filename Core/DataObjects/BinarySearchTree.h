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
#include "Core/Interfaces/icomparer.h"
using namespace std;

#ifndef GUTIL_BST_H
#define GUTIL_BST_H

GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements an AVL-balanced binary search tree.

    BST's have the advantage of storing the data so it is always sorted,
    and can be looked up in O(log(n)) time.  Insertions, removals and lookups
    all are O(log).  Use this class to maintain a fast index of any kind of data.

    Implement your own compare function to control how the data is indexed.
*/
template<class T>class BinarySearchTree
{
    template<class> friend class BST_NodeIterator;
public:

    /** Creates an empty BST with the compare function.
        \param cmp A comparison object to determine the sort order of the tree.
        If left 0 it will use the default compare function, the less-than operator.
        The tree will take ownership of this pointer and delete it when finished.
    */
    BinarySearchTree(GUtil::Core::Interfaces::IComparer<T> *cmp = 0);


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
    bool Contains(const T &);


    /** Does a lookup on the given object and returns a const reference to its version of it.
        Throws an exception if not found.
    */
    const T &Search(const T &obj);


    /** A class for iterating through the BinarySearchTree.

        Because it's a BST, the elements will always be sorted when you traverse it depth-first.

        There is no non-const iterator, because you are not allowed to modify the elements of the BST
        (it would destroy the tree's index).  To do this you have to add or remove items from the tree.
    */
    class const_iterator :
            public bst_node_df_iterator
    {
    public:
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;

        /** Constructs an invalid iterator. */
        inline const_iterator()
            :cmp(0)
        {}
        inline const_iterator(bst_node *n, GUtil::Core::Interfaces::IComparer<T> *compare)
            :bst_node_df_iterator(n),
              cmp(compare)
        {}
        inline const_iterator(const const_iterator &o)
            :bst_node_df_iterator(o),
              cmp(o.cmp)
        {}

        /** Dereference the iterator and return a reference to the data. */
        inline const T &operator*() const { return *(reinterpret_cast<T *>(current->Data)); }
        /** Dereference the iterator and return a pointer to the data. */
        inline const T *operator->() const { return reinterpret_cast<T *>(current->Data); }

        /** Prefix ++.  Throws an exception if you can't advance. */
        inline const_iterator &operator ++(){
            advance();
            return *this;
        }
        /** Postfix ++.  Throws an exception if you can't advance. */
        const_iterator operator ++(int){
            const_iterator ret(*this);
            advance();
            return ret;
        }
        /** Prefix --.  Throws an exception if you can't advance. */
        const_iterator &operator --(){
            retreat();
            return *this;
        }
        /** Postfix --.  Throws an exception if you can't advance. */
        const_iterator operator --(int){
            const_iterator ret(*this);
            retreat();
            return ret;
        }

        /** Comparison operator */
        bool operator < (const const_iterator &o) const{
            if(current && o.current)
            {
                return cmp->Compare(*reinterpret_cast<T*>(current->Data),
                                    *reinterpret_cast<T*>(o.current->Data)) < 0;
            }
            else
            {
                if(current == 0 && o.current == 0){
                    // I am at the beginning, o is at the end
                    if(mem_begin && o.mem_end)
                        return true;
                    return false;
                }
                else if(current == 0)
                {
                    // I am at the beginning, so I come before o
                    return mem_begin;
                }
                else
                {
                    // o is at the end, so I come before it
                    return o.mem_end;
                }
            }

        }
        /** Comparison operator. */
        bool operator > (const const_iterator &o) const{ return !(*this <= o); }
        /** Comparison operator. */
        bool operator <= (const const_iterator &o) const{ return *this == 0 || *this < o; }
        /** Comparison operator. */
        bool operator >= (const const_iterator &o) const{ return !(*this < o); }

    private:
        GUtil::Core::Interfaces::IComparer<T> *cmp;
    };
    friend class const_iterator;


    /** Returns an iterator starting at the first element in the tree. */
    inline const_iterator begin() const{
        return const_iterator(root ? root->LeftmostChild : 0, cmp);
    }
    /** Returns an iterator starting at the end of the tree.  You must decrement it before
        it points to a valid entry.
    */
    const_iterator end() const{
        const_iterator ret;
        if(root)
            ret = ++const_iterator(root ? root->RightmostChild : 0, cmp);
        return ret;
    }
    /** Returns an iterator starting before the first element in the tree. */
    inline const_iterator preBegin() const{
        return --const_iterator(root ? root->LeftmostChild : 0, cmp);
    }

    /** Returns how many items are in the BST */
    inline long size() const{
        return m_size;
    }

    /** Returns the least element in the tree.  Throws an exception
        if there are no items in the tree
    */
    inline const T &min() const{
        if(root)
            return *reinterpret_cast<T *>(root->LeftmostChild->Data);
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "There are no elements in the BST");
    }

    /** Returns the greatest element in the tree.  Throws an exception
        if there are no items in the tree.
    */
    inline const T &max() const{
        if(root)
            return *reinterpret_cast<T *>(root->RightmostChild->Data);
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "There are no elements in the BST");
    }


protected:

    bst_node *root;
    GUtil::Core::Interfaces::IComparer<T> *cmp;


private:

    static void cleanup_memory(bst_node *);

    // After the root node potentially moved, use this function to find the new root.
    void _update_root_node();

    long m_size;

    /** You cannot copy a BST.  You must clone it, which is a N*log(N) process. */
    BinarySearchTree(const BinarySearchTree<T> &){}
    /** You cannot copy a BST.  You must clone it, which is a N*log(N) process. */
    BinarySearchTree<T> &operator =(const BinarySearchTree<T> &){}

};


template<class T>void BinarySearchTree<T>::Add(const T &object)
{
    if(root)
    {
        // Find the place to insert, has to be a leaf node
        bst_node *cur( root );
        bst_node *next_cur( root );
        bst_node::SideEnum insertion_side(bst_node::LeftSide);
        while(next_cur)
        {
            cur = next_cur;
            int cmp_res( cmp->Compare(object, *((T*)cur->Data)) );

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
        new_node->Data = reinterpret_cast<void *>(new T(object));

        bst_node::Insert(cur, new_node, insertion_side);
        _update_root_node();
    }
    else
    {
        // If the root is null, then this is the first item in the tree (easy case)
        root = new bst_node;
        root->Data = reinterpret_cast<void *>(new T(object));
    }

    m_size++;
}

template<class T>bool BinarySearchTree<T>::Contains(const T &object)
{
    bst_node *cur( root );
    while(cur)
    {
        int cmp_res( cmp->Compare(object, *((T*)cur->Data)) );

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
        int cmp_res( cmp->Compare(object, *((T*)cur->Data)) );

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
        int cmp_res( cmp->Compare(object, *reinterpret_cast<T*>(cur->Data)) );

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

        // Find a replacement node
        bst_node *replacement(0);
        if(cur->Height > 0)
        {
            if(cur->HeightDifference() > 0)
            {
                // Left-heavy
                replacement = cur->LChild->RightmostChild;
            }
            else
            {
                // Right-heavy
                replacement = cur->RChild->LeftmostChild;
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

        delete reinterpret_cast<T *>(cur->Data);
        bst_node::Delete(cur, replacement);

        _update_root_node();

        m_size--;
    }
}



/** The default compare is the less-than operator. */
template<class T>class Default_BST_Comparer :
        public GUtil::Core::Interfaces::IComparer<T>
{
public:
    int Compare(const T &lhs, const T &rhs) const{
        if(lhs < rhs)
            return -1;
        else if(rhs < lhs)
            return 1;
        return 0;
    }
};

template<class T>BinarySearchTree<T>::BinarySearchTree(GUtil::Core::Interfaces::IComparer<T> *ic)
    :root(0),
      cmp(ic ? ic : new Default_BST_Comparer<T>),
      m_size(0)
{}

template<class T>BinarySearchTree<T>::~BinarySearchTree()
{
    Clear();
    delete cmp;
}

template<class T>void BinarySearchTree<T>::Clear()
{
    if(root)
    {
        // Need to iterate through the nodes and delete all the memory!  The bst_node
        //  can't do it because it stores the data as void *
        //cleanup_memory(root);
        root = 0;

        m_size = 0;
    }
}

template<class T>void BinarySearchTree<T>::cleanup_memory(bst_node *n)
{
    if(n)
    {
        cleanup_memory(n->LChild);
        cleanup_memory(n->RChild);
        delete reinterpret_cast<T *>(n->Data);
    }
}

template<class T>void BinarySearchTree<T>::_update_root_node()
{
    bst_node *n(root);
    while((n = n->Parent))
        root = n;
}












/** A class for iterating manually through the binary search tree.

    This iterator is not STL compliant, because there is no STL compliant way
    to do this.  It's useful for debugging, but not much else.  You shouldn't normally
    have to use this class.  Because it's a template, if you don't use it
    it won't build into your application.
*/
template<class T> class BST_NodeIterator
{
public:
    BST_NodeIterator(const BinarySearchTree<T> &t)
        :m_node(t.root)
    {}

    /** The data held in this node. */
    const T &Value() const{
        return *reinterpret_cast<T *>(m_node->Data);
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


GUTIL_END_CORE_NAMESPACE

#endif //GUTIL_BST_H
