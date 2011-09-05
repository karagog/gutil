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
#include "bst_node.h"
#include <iterator>
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** The BinarySearchTree non-templated base class.
    It is implemented this way so more code is baked into the library,
    improving code size and build times.
*/
class bst_t
{
    GUTIL_DISABLE_COPY(bst_t);
public:

    /** A class for iterating through the bst_t.

        Because it's a BST, the elements will always be sorted when you traverse it depth-first.

        There is no non-const iterator, because you are not allowed to modify the elements of the BST
        (it would destroy the tree's index).  To do this you have to add or remove items from the tree.

        \note I'm convinced iteration can be done in constant time, but my algorithm is O(log(n))
        in the worst case and O(1) in the best case, but I am trying to improve it.
    */
    class const_iterator
    {
        friend class bst_t;
    public:
        typedef std::bidirectional_iterator_tag iterator_category;

        /** Constructs an invalid iterator, which also happens to be equal to end. */
        const_iterator();
        explicit const_iterator(bst_node *, const bst_t &);
        const_iterator(const const_iterator &o);

        /** Prefix ++.  Throws an exception if you can't advance. */
        const_iterator &operator ++();

        /** Postfix ++.  Throws an exception if you can't advance. */
        const_iterator operator ++(int);

        /** Prefix --.  Throws an exception if you can't advance. */
        const_iterator &operator --();

        /** Postfix --.  Throws an exception if you can't advance. */
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
        bool operator != (const const_iterator &) const;

        bst_node *operator->();
        const bst_node *operator->() const;
        const bst_node &operator *() const;
        bst_node &operator *();

        /** Returns true if the iterator points to a valid element. */
        operator bool() const;

    protected:

        bst_node *current;


    private:
        GUtil::Core::Interfaces::IVoidComparer *cmp;

        /** Advance the iterator.  Throws an exception if you can't advance. */
        void advance();
        /** Advance the iterator in reverse order.  Throws an exception if you can't advance. */
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
    {};

    /** This class is desiged to be inherited by BinarySearchTree, so no public constructor */
    bst_t(void_wrapper *);
    ~bst_t();

    bst_node *add(const void *const);
    bst_node *search(const void *const) const;

    /** Enables you to conduct a search by something other than the normal type held by the
        BST.  So you could search with a string to find an int, for example if you reinterpret
        the void * in the void wrapper.
        \note In this case the void_wrapper is not owned by the BST, so you must delete it yourself.
    */
    bst_node *search(const void *const, const void_wrapper *) const;

    bool remove(const void *const);
    bool remove(const const_iterator &);

    /** Returns the least element of the tree. */
    bst_node *first() const;
    /** Returns the most element of the tree */
    bst_node *last() const;

    /** Our friends have access to our compare function. */
    Interfaces::IVoidComparer *cmp;

    /** Our friends have access to our root. */
    bst_node *root;


private:

    void_wrapper *data_access_wrapper;

    // After the root node potentially moved, use this function to find the new root.
    void _update_root_node();
    void _cleanup_memory(bst_node *n);

    static void walk_parents_update_heights_rebalance(bst_node *);
    static void refresh_node_state(bst_node *);

    static void rotate_right(bst_node *parent);
    static void rotate_left(bst_node *parent);

    /** Rebalances a node.  Does nothing if the node is already balanced. */
    static void rebalance(bst_node *);

    long m_size;

};


GUTIL_END_CORE_NAMESPACE;

#endif // BST_T_H
