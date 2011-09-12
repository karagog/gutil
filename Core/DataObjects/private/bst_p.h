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

#ifndef BST_T_H
#define BST_T_H

#include "Core/Interfaces/ivoidwrappers.h"
#include "binary_tree_node.h"
#include "gutil_globals.h"
#include <iterator>
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** The Binary Search Tree non-templated base class.
    It is implemented this way so more code is baked into the library,
    improving code size and build times.
*/
class bst_p
{
    GUTIL_DISABLE_COPY(bst_p);
public:

    /** Represents a single node of the binary search tree.

        \sa Set
    */
    class bst_node :
            public binary_tree_node
    {
    public:
        /** Constructs a default node and points the leftmost and rightmost children to itself. */
        inline bst_node()
            :LeftmostChild(this),
              RightmostChild(this),
              Height(0)
        {}

        /** A reference to my most leftist child (min of my children).
            This makes it possible to get to the begin() and end() of the tree
            in constant time, and also makes iteration much faster
        */
        bst_node *LeftmostChild;

        /** A reference to my most rightist child (max of my children).
            This makes it possible to get to the begin() and end() of the tree
            in constant time, and also makes iteration much faster
        */
        bst_node *RightmostChild;

        /** The height of this node, which is 1 + the max of the heights of my two children */
        int Height;

        /** The difference in height of my two children (left - right) */
        inline int HeightDifference() const{
            int sum(0);
            if(LChild) sum += (1 + static_cast<bst_node *>(LChild)->Height);
            if(RChild) sum -= (1 + static_cast<bst_node *>(RChild)->Height);
            return sum;
        }

        /** Returns whether the node is balanced, or in other words whether its children are not
            more than 1 node different in height.
        */
        inline bool Balanced() const{
            return gAbs(HeightDifference()) <= 1;
        }

    };


    /** A class for iterating through the bst_t.

        Because it's a BST, the elements will always be sorted when you traverse it depth-first.

        There is no non-const iterator, because you are not allowed to modify the elements of the BST
        (it would destroy the tree's index).  To do this you have to add or remove items from the tree.

        \note Iteration is done in amortized constant time O(1).
    */
    class const_iterator
    {
        friend class bst_p;
    public:
        typedef std::bidirectional_iterator_tag iterator_category;

        /** Constructs an invalid iterator, which also happens to be equal to end. */
        const_iterator();
        explicit const_iterator(bst_node *, const Interfaces::IVoidComparer *const);

        /** Prefix ++.  \note O(1) */
        const_iterator &operator ++();

        /** Postfix ++.  \note O(1) */
        const_iterator operator ++(int);

        /** Prefix --.  \note O(1) */
        const_iterator &operator --();

        /** Postfix --.  \note O(1) */
        const_iterator operator --(int);

        /** Comparison operator */
        bool operator < (const const_iterator &o) const;

        /** Comparison operator. */
        bool operator > (const const_iterator &o) const;

        /** Comparison operator. */
        bool operator <= (const const_iterator &o) const;

        /** Comparison operator. */
        bool operator >= (const const_iterator &o) const;

        bool operator == (const const_iterator &) const;
        inline bool operator != (const const_iterator &o) const{ return !(*this == o); }

        bst_node *operator->();
        const bst_node *operator->() const;
        const bst_node &operator *() const;
        bst_node &operator *();

        /** Returns true if the iterator points to a valid element. */
        operator bool() const;

    protected:

        bst_node *current;


    private:
        Interfaces::IVoidComparer const* cmp;

        /** Advance the iterator.  Does nothing if you can't advance. */
        void advance();
        /** Advance the iterator in reverse order.  Does nothing if you can't advance. */
        void retreat();

        bst_node *mem_begin;
        bst_node *mem_end;
    };
    friend class const_iterator;

    /** Clears all memory and deletes all objects.
        \note O(N)
    */
    void Clear();

    /** Returns how many items are in the BST */
    inline long Size() const{
        return m_size;
    }


protected:

    /** An abstract class that declares an interface to our void pointers that we require from
        the derived class.

        It must be injected via our constructor by our derived classes.
    */
    class void_wrapper :
            public Interfaces::IVoidComparer,
            public Interfaces::IVoidCopyer,
            public Interfaces::IVoidDeleter
    {
    public:
        /** So you can be deleted by this interface. */
        virtual ~void_wrapper();
    };

    /** This class is desiged to be inherited, so no public constructor */
    bst_p(void_wrapper *);
    /** Deallocates all memory.
        \note O(N)
    */
    ~bst_p();

    bst_node *add(const void *const);
    bst_node *search(const void *const) const;

    /** Enables you to conduct a search by something other than the normal type held by the
        BST.  So you could search with a string to find an int, for example if you reinterpret
        the void * in the void wrapper.
        \note In this case the void comparer is not owned by the BST, so you must delete it yourself.
    */
    bst_node *search(const void *const, const Interfaces::IVoidComparer *) const;

    bool remove(const void *const);
    bool remove(const const_iterator &);

    /** Returns the least element of the tree. */
    bst_node *first() const;
    /** Returns the most element of the tree */
    bst_node *last() const;

    /** Our friends have access to our root. */
    bst_node *root;

    /** Derived classes may need to access our type wrapper. */
    void_wrapper *data_access_wrapper;

private:

    // After the root node potentially moved, use this function to find the new root.
    void _update_root_node();
    void _cleanup_memory(binary_tree_node *n);

    static void walk_parents_update_heights_rebalance(bst_node *, bool already_rebalanced = false);
    static void refresh_node_state(bst_node *);

    static void rotate_right(binary_tree_node *parent);
    static void rotate_left(binary_tree_node *parent);

    /** Rebalances a node.  Does nothing if the node is already balanced. */
    static void rebalance(binary_tree_node *);

    long m_size;

};


GUTIL_END_CORE_NAMESPACE;

#endif // BST_T_H
