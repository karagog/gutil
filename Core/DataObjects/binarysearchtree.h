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

#include "gassert.h"
#include "Core/exception.h"
#include "Core/DataObjects/flexibletypecomparer.h"
using namespace std;

#ifndef GUTIL_BST_H
#define GUTIL_BST_H

GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements an AVL-balanced binary search tree.

    BST's have the advantage of storing the data so it is always sorted,
    and can be looked up in O(log(n)) time.  Insertions, removals and lookups
    all are O(log(n)).  Use this class to maintain a fast index of any kind of data.

    Implement your own compare function to control how the data is indexed.
*/
template<class T, class KeyType = T>class BinarySearchTree
{
    class node
    {
    public:
        /** A reference to the parent of this node. */
        node *Parent;

        /** A reference to the left child of this node. */
        node *LChild;

        /** A reference to the right child of this node. */
        node *RChild;

        /** The height of this node, which is 1 + the max of the heights of my two children */
        int Height;

        /** A reference to my most leftist child (min of my children).
            This makes it possible to get to the begin() and end() of the tree
            in constant time, and also makes iteration much faster
        */
        node *LeftmostChild;

        /** A reference to my most rightist child (max of my children).
            This makes it possible to get to the begin() and end() of the tree
            in constant time, and also makes iteration much faster
        */
        node *RightmostChild;

        /** The data, of type T, contained in the node. */
        T Data;

        /** The difference in height of my two children (left - right) */
        inline int HeightDifference() const{
            int sum(0);
            if(LChild) sum += (1 + static_cast<node *>(LChild)->Height);
            if(RChild) sum -= (1 + static_cast<node *>(RChild)->Height);
            return sum;
        }

        /** Returns whether the node is balanced, or in other words whether its children are not
            more than 1 node different in height.
        */
        inline bool Balanced() const{
            return gAbs(HeightDifference()) <= 1;
        }

        /** Constructs a node with the given parent. */
        inline node(const T &data, node *parent = 0)
            :Parent(parent),
              LChild(0),
              RChild(0),
              Height(0),
              LeftmostChild(this),
              RightmostChild(this),
              Data(data)
        {}

        inline ~node(){
            if(LChild) delete LChild;
            if(RChild) delete RChild;
        }


        /** Gives a way to specify the side of a node. */
        enum SideEnum
        {
            /** Represents a null value */
            NoSide,
            /** The left child of a node. */
            LeftSide,
            /** The right child of a node. */
            RightSide
        };

        /** Returns which side of my parent I'm on.  Merely a convenience function. */
        inline SideEnum SideOfParent() const
        {
            SideEnum ret(NoSide);
            if(Parent)
            {
                if(Parent->LChild == this)          ret = LeftSide;
                else if(Parent->RChild == this)     ret = RightSide;
            }
            return ret;
        }

    };

    class iterator_base
    {
    public:

        bool operator == (const iterator_base &o) const{
            return current == o.current &&
                    (current != 0 ||
                    ((m_begin == 0 && m_end == 0) || (o.m_begin == 0 && o.m_end == 0) ||
                     (m_begin == o.m_begin && m_end == o.m_end)));
        }
        inline bool operator != (const iterator_base &o) const{ return !(*this == o); }

        /** Returns true if the iterator points to a valid element. */
        inline operator bool() const{ return current; }

        /** Dereference the iterator and return a reference to the data. */
        inline const T &operator*() const { return current->Data; }
        /** Dereference the iterator and return a pointer to the data. */
        inline const T *operator->() const { return &current->Data; }

        /** Manipulates the iterator manually, moving it to its right-child.
            This is useful for iterating breadth-first.
        */
        inline void MoveRight(){
            m_parent = current;
            current = current->RChild;
        }
        inline bool CanMoveRight() const{ return current ? current->RChild : false; }
        /** Manipulates the iterator manually, moving it to its left-child.
            This is useful for iterating breadth-first.
        */
        inline void MoveLeft(){
            m_parent = current;
            current = current->LChild;
        }
        inline bool CanMoveLeft() const{ return current ? current->LChild : false; }
        inline void MoveUp(){
            current = m_parent;
            m_parent = current->Parent;
        }
        inline bool CanMoveUp() const{ return m_parent; }

    protected:

        node *current;

        node *m_begin;
        node *m_end;

        node *m_parent;

        inline iterator_base(node *n = 0)
            :current(n), m_begin(0), m_end(0), m_parent(0){}
        inline iterator_base(const iterator_base &o)
            :current(o.current), m_begin(o.m_begin), m_end(o.m_end), m_parent(o.m_parent){}

        /** Advance the iterator.  Does nothing if you can't advance. */
        virtual void advance()
        {
            if(current)
            {
                if(current->RChild)
                    current = current->RChild->LeftmostChild;
                else
                {
                    node *jump_to(current);
                    while(jump_to && jump_to->SideOfParent() != node::LeftSide)
                        jump_to = jump_to->Parent;

                    if(jump_to)
                        current = jump_to->Parent;
                    else
                    {
                        // We've hit the end of the BST
                        m_end = current;
                        current = 0;
                    }
                }
            }
            else if(m_begin)
            {
                current = m_begin;
                m_begin = 0;
            }
        }

        /** Advance the iterator in reverse order.  Does nothing if you can't advance. */
        virtual void retreat()
        {
            if(current)
            {
                if(current->LChild)
                    current = current->LChild->RightmostChild;
                else
                {
                    node *jump_to(current);
                    while(jump_to && jump_to->SideOfParent() != node::RightSide)
                        jump_to = jump_to->Parent;

                    if(jump_to)
                        current = jump_to->Parent;
                    else
                    {
                        // We've hit the beginning of the BST
                        m_begin = current;
                        current = 0;
                    }
                }
            }
            else if(m_end)
            {
                current = m_end;
                m_end = 0;
            }
        }

    };

public:

    class Comparer :
            public FlexibleTypeComparer<KeyType>
    {
    public:

        inline Comparer(){}
        inline Comparer(int (*cmp)(const KeyType &, const KeyType &))
            :FlexibleTypeComparer<KeyType>(cmp)
        {}

    };

    class Converter
    {
    public:
        /** \note You can only use this constructor if KeyType == T, otherwise you must provide
            your own conversion from T to the KeyType.
        */
        inline Converter()
        {
            m_convert = &default_convert;
        }
        inline Converter(KeyType const &(*convert)(T const &))
        {
            m_convert = convert;
        }

        /** Use the parenthesis operator to dereference the conversion function. */
        inline KeyType const &operator()(T const &t) const{ return m_convert(t); }


    private:

        KeyType const &(*m_convert)(T const &);

        static KeyType const &default_convert(T const &o){ return o; }

    };

    inline BinarySearchTree()
        :m_size(0),
          root(0)
    {}

    /** Creates an empty BST with the default type wrapper.
        \param cmp A comparison object to determine the sort order of the tree.
        If left 0 it will use the default compare function, the less-than operator.
        The tree will take ownership of this pointer and delete it when finished.
        \sa BST_Type_Wrapper
    */
    inline explicit BinarySearchTree(const Comparer &c)
        :m_size(0),
          root(0),
          compare(c)
    {}

    inline explicit BinarySearchTree(const Comparer &c, const Converter &conv)
        :m_size(0),
          root(0),
          compare(c),
          convert(conv)
    {}

    /** Performs a deep copy of the other tree.
        \note O(N Log(N))
    */
    inline BinarySearchTree(const BinarySearchTree<T, KeyType> &o)
        :m_size(0),
          root(0),
          compare(o.compare),
          convert(o.convert)
    {
        BinarySearchTree<T, KeyType>::const_iterator iter(o.begin());
        while(iter)
        {
            Add(*iter);
            ++iter;
        }
    }

    inline ~BinarySearchTree(){ Clear(); }

    /** For iterating depth-first through the BST. */
    class iterator :
            public iterator_base
    {
        friend class BinarySearchTree;
    public:

        inline iterator(){}
        inline iterator(const iterator_base &o) :iterator_base(o){}

        inline iterator &operator =(const iterator_base &o){ ::new(this) iterator(o); }

        /** Prefix ++.  \note O(1) */
        inline iterator &operator ++(){ this->advance(); return *this; }

        /** Postfix ++.  \note O(1) */
        inline iterator operator ++(int){ iterator ret(*this); this->advance(); return ret; }

        /** Prefix --.  \note O(1) */
        inline iterator &operator --(){ this->retreat(); return *this; }

        /** Postfix --.  \note O(1) */
        inline iterator operator --(int){ iterator ret(*this); this->retreat(); return ret; }


    protected:

        inline iterator(node *n) :iterator_base(n) {}

    };

    /** For iterating depth-first through the BST. */
    class const_iterator :
            public iterator_base
    {
        friend class BinarySearchTree;
    public:

        inline const_iterator(){}
        inline const_iterator(const iterator_base &o) :iterator_base(o){}

        inline const_iterator &operator =(const const_iterator &o){ ::new(this) const_iterator(o); }

        /** Prefix ++.  \note O(1) */
        inline const_iterator &operator ++(){ this->advance(); return *this; }

        /** Postfix ++.  \note O(1) */
        inline const_iterator operator ++(int){ const_iterator ret(*this); this->advance(); return ret; }

        /** Prefix --.  \note O(1) */
        inline const_iterator &operator --(){ this->retreat(); return *this; }

        /** Postfix --.  \note O(1) */
        inline const_iterator operator --(int){ const_iterator ret(*this); this->retreat(); return ret; }


    protected:

        inline const_iterator(node *n) :iterator_base(n) {}

    };

    /** Makes a copy of object and puts in the tree.
        If already exists in the tree an exception is thrown.
        \note O(log(N))
    */
    void Add(const T &object);


    /** Removes the object from the tree.  Does nothing if object not in the tree.
        \returns True if item removed
        \note O(log(N))
    */
    inline void Remove(const T &object){ _remove( Search(object).current ); }

    /** Removes the object from the tree.  Does nothing if object not in the tree.
        \returns True if item removed
        \note O(log(N))
    */
    inline void Remove(const iterator &iter){ _remove(iter.current); }

    /** Removes all items in the tree
        \note O(N)
    */
    void Clear()
    {
        if(root)
        {
            _cleanup_memory(root);
            root = 0;
            m_size = 0;
        }
    }


    /** Does a lookup on the given object and returns an iterator to it.
        Returns an invalid iterator equal to end() if not found.
        \note O(log(N))
    */
    const_iterator Search(const T &object) const{
        const_iterator ret(root);
        while(ret)
        {
            const int cmp_res( compare(convert(object), convert(ret.current->Data)) );
            if(cmp_res < 0)         ret.MoveLeft();
            else if(cmp_res > 0)    ret.MoveRight();
            else break;
        }
        return ret;
    }

    /** Does a lookup on the given object and returns an iterator to it.
        Returns an invalid iterator equal to end() if not found.
        \note O(log(N))
    */
    iterator Search(const T &object){
        iterator ret(root);
        while(ret)
        {
            const int cmp_res( compare(convert(object), convert(ret.current->Data)) );
            if(cmp_res < 0)         ret.MoveLeft();
            else if(cmp_res > 0)    ret.MoveRight();
            else break;
        }
        return ret;
    }

    /** Does a lookup with the provided key, which can be a different type than T,
        and returns an iterator to it.  You must provide your own void comparer which knows
        how to interpret the new type and compare it with type T.

        Returns an invalid iterator equal to end() if not found.
        \note O(log(N))
    */
    template<class KT>iterator Search(const KT &object){
        iterator ret( root );
        while(ret)
        {
            const int cmp_res( compare(object, convert(ret.current->Data)) );
            if(cmp_res < 0)         ret.MoveLeft();
            else if(cmp_res > 0)    ret.MoveRight();
            else break;
        }
        return ret;
    }

    template<class KT>const_iterator Search(const KT &object) const{
        const_iterator ret( root );
        while(ret)
        {
            const int cmp_res( compare(object, convert(ret.current->Data)) );
            if(cmp_res < 0)         ret.MoveLeft();
            else if(cmp_res > 0)    ret.MoveRight();
            else break;
        }
        return ret;
    }

    /** Returns an iterator starting at the root of the tree. */
    inline iterator Root(){ return root; }
    /** Returns an iterator starting at the root of the tree. */
    inline const_iterator Root() const{ return root; }

    /** A convenience function which returns if the object exists in the tree.
        \note O(log(N))
    */
    inline bool Contains(const T &object) const{ return Search(object); }

    /** Returns how many items are in the BST */
    inline GUINT32 Size() const{ return m_size; }


    /** Returns an iterator starting at the first element in the tree.
        \note O(1)
    */
    inline iterator begin(){ return iterator(_first()); }
    inline const_iterator begin() const{ return iterator(_first()); }

    /** Returns an iterator starting at the end of the tree.
        \note O(1)
    */
    inline const_iterator end() const{ return Size() > 0 ? ++const_iterator(_last()) : const_iterator(); }

    /** Returns an iterator starting at the end of the tree.
        \note O(1)
    */
    inline iterator end(){ return Size() > 0 ? ++iterator(_last()) : iterator(); }

    /** Returns an iterator starting at the last element in the tree.
        \note O(1)
    */
    inline const_iterator rbegin() const{ return Size() > 0 ? const_iterator(_last()) : const_iterator(); }

    /** Returns an iterator starting at the last element in the tree.
        \note O(1)
    */
    inline iterator rbegin(){ return Size() > 0 ? iterator(_last()) : iterator(); }

    /** Returns an iterator starting before the first element in the tree.
        \note O(1)
    */
    inline const_iterator rend() const{ return Size() > 0 ? --const_iterator(_first()) : const_iterator(); }

    /** Returns an iterator starting before the first element in the tree.
        \note O(1)
    */
    inline iterator rend(){ return Size() > 0 ? --iterator(_first()) : iterator(); }


    /** Returns the least element in the tree.  Throws an exception
        if there are no items in the tree
        \note O(1)
    */
    inline const T &min() const{
        if(Size() == 0)
            THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                       "There are no elements in the BST");
        return _first()->Data;
    }

    /** Returns the greatest element in the tree.  Throws an exception
        if there are no items in the tree.
        \note O(1)
    */
    inline const T &max() const{
        if(Size() == 0)
            THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                       "There are no elements in the BST");
        return _last()->Data;
    }


private:

    GUINT32 m_size;
    node *root;
    Comparer compare;
    Converter convert;

    void _remove(node *);

    inline node *_first() const{ return root ? root->LeftmostChild : 0; }
    inline node *_last() const{ return root ? root->RightmostChild : 0; }

    static void _cleanup_memory(node *n)
    {
        if(n->LChild)
        {
            _cleanup_memory(n->LChild);
            n->LChild = 0;
        }
        if(n->RChild)
        {
            _cleanup_memory(n->RChild);
            n->RChild = 0;
        }
        delete n;
    }

    inline void _update_root_node()
    {
        node *n(root);
        while(n && (n = n->Parent)) root = n;
    }

    void _walk_parents_update_heights_rebalance(node *n, bool already_balanced = false)
    {
        if(!n) return;

        _refresh_node_state(n);

        // Rebalance the node if it's unbalanced
        if(!already_balanced && !n->Balanced())
        {
            _rebalance(n);
            already_balanced = true;
        }

        _walk_parents_update_heights_rebalance(static_cast<node *>(n->Parent), already_balanced);
    }

    void _refresh_node_state(node *n)
    {
        // Update the node's height cache
        if(!n->LChild && !n->RChild)
            n->Height = 0;
        else
        {
            n->Height = gMax(n->LChild ? n->LChild->Height : 0,
                             n->RChild ? n->RChild->Height : 0) + 1;
        }

        // Update the left-most and right-most child caches
        n->LeftmostChild = n->LChild ? n->LChild->LeftmostChild : n;
        n->RightmostChild = n->RChild ? n->RChild->RightmostChild : n;
    }

    void _rotate_right(node *n)
    {
        node *parent(n->Parent);
        if(parent)
        {
            if(parent->LChild == n)
                parent->LChild = n->LChild;
            else
                parent->RChild = n->LChild;
        }
        n->LChild->Parent = parent;

        node *tmp(n->LChild->RChild);
        n->Parent = n->LChild;
        n->LChild->RChild = n;
        n->LChild = tmp;
        if(tmp)
            tmp->Parent = n;

        // Have to refresh the node we just rotated, the other one will be refreshed
        //  automatically when we walk up the tree
        _refresh_node_state(n);
    }

    void _rotate_left(node *n)
    {
        node *parent(n->Parent);
        if(parent)
        {
            if(parent->LChild == n)
                parent->LChild = n->RChild;
            else
                parent->RChild = n->RChild;
        }
        n->RChild->Parent = parent;

        node *tmp(n->RChild->LChild);
        n->Parent = n->RChild;
        n->RChild->LChild = n;
        n->RChild = tmp;
        if(tmp)
            tmp->Parent = n;

        // Have to refresh the node we just rotated, the other one will be refreshed
        //  automatically when we walk up the tree
        _refresh_node_state(n);
    }

    void _rebalance(node *n)
    {
        const int height_difference = n->HeightDifference();
        if(height_difference > 1)
        {
            // The node is left-heavy

            // Check if it's LR imbalance so we can resolve that first.
            if(n->LChild->HeightDifference() < 0)
                _rotate_left(n->LChild);

            // Now that the LR imbalance is fixed, do the LL rebalance.
            _rotate_right(n);
        }
        else if(height_difference < -1)
        {
            // The node is right-heavy

            // Check if it's RL imbalance so we can resolve that first.
            if(n->RChild->HeightDifference() > 0)
                _rotate_right(n->RChild);

            // Now that the RL imbalance is fixed, do the RR rebalance.
            _rotate_left(n);
        }
    }

};






template<class T, class KeyType>void BinarySearchTree<T, KeyType>::_remove(node *n)
{
    // Remove it.  The deletion algorithm goes as follows:
    //  I need to find a replacement from the leaves of the tree to
    //  come take my place, then I will rebalance from the bottom of the tree.
    //  I will find a replacement as the left-most item on my right side, or my
    //  right-most item on my left side, whichever one is taller.

    // Find a replacement node
    node *replacement(0);
    if(n->Height > 0)
    {
        if(n->HeightDifference() > 0)
        {
            // Left-heavy
            replacement = static_cast<node *>(n->LChild)->RightmostChild;
        }
        else
        {
            // Right-heavy
            replacement = static_cast<node *>(n->RChild)->LeftmostChild;
        }
    }


    if(root == n)
    {
        // We have to pre-emptively adjust our root node, because we will be unable to
        //  find the new root once it's deleted.
        if(root->RChild)
            root = static_cast<node *>(root->RChild);
        else if(root->LChild)
            root = static_cast<node *>(root->LChild);
        else
            root = 0;
    }

    // This variable determines where to start adjusting the node height after deletion.
    node *start_height_adjustment(0);

    if(replacement)
    {
        // If the replacement has a child (at most 1) then we move it into the replacement's place
        node *replacement_child(0);
        if(replacement->RChild)
            replacement_child = static_cast<node *>(replacement->RChild);
        else if(replacement->LChild)
            replacement_child = static_cast<node *>(replacement->LChild);

        if(n == replacement->Parent)
            start_height_adjustment = replacement;
        else
        {
            start_height_adjustment = static_cast<node *>(replacement->Parent);
            switch(replacement->SideOfParent())
            {
            case node::RightSide:
                replacement->Parent->RChild = replacement_child;
                break;
            case node::LeftSide:
                replacement->Parent->LChild = replacement_child;
                break;
            default:
                break;
            }

            if(replacement_child)
                replacement_child->Parent = replacement->Parent;
        }

        replacement->Parent = n->Parent;
        if(replacement != n->RChild)
            replacement->RChild = n->RChild;
        if(replacement != n->LChild)
            replacement->LChild = n->LChild;
    }
    else
    {
        start_height_adjustment = static_cast<node *>(n->Parent);
    }


    if(n->RChild && n->RChild != replacement)
        n->RChild->Parent = replacement;
    if(n->LChild && n->LChild != replacement)
        n->LChild->Parent = replacement;
    if(n->Parent)
    {
        switch(n->SideOfParent())
        {
        case node::RightSide:
            n->Parent->RChild = replacement;
            break;
        case node::LeftSide:
            n->Parent->LChild = replacement;
            break;
        default:
            break;
        }
    }

    // Delete the node (set children to 0 so to not delete them)
    n->LChild = 0;
    n->RChild = 0;
    delete n;

    // Walk up the tree and update the height variables.
    _walk_parents_update_heights_rebalance(start_height_adjustment);

    _update_root_node();

    --m_size;
}

template<class T, class KeyType>void BinarySearchTree<T, KeyType>::Add(const T &object)
{
    if(root)
    {
        // Find the place to insert, has to be a leaf node
        iterator iter( Search(object) );
        if(iter)
        {
            // There's an insertion collision
            THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                       "Object already exists in BST.");
        }

        node *new_node( new node(object, iter.m_parent) );
        const int cmp_res( compare(convert(object), convert(iter.m_parent->Data)) );
        if(cmp_res < 0)
            new_node->Parent->LChild = new_node;
        else if(cmp_res > 0)
            new_node->Parent->RChild = new_node;
        else
            GASSERT(false);

        // Now we need to ascend the tree to the root and update the heights
        _walk_parents_update_heights_rebalance(new_node->Parent);

        _update_root_node();
    }
    else
    {
        // If the root is null, then this is the first item in the tree (easy case)
        root = new node(object);
    }

    m_size++;
}


GUTIL_END_CORE_NAMESPACE

#endif //GUTIL_BST_H
