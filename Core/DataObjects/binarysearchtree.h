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
#include "private/bst_t.h"
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
template<class T>class BinarySearchTree :
        public bst_t
{
    GUTIL_DISABLE_COPY(BinarySearchTree<T>);
    template<class> friend class BST_NodeIterator;
public:

    /** A type wrapper to satisfy the injection constraints of the BinarySearchTree.

        By default it provides an allocator, deallocator and compare function, all of which you can
        override to perform your own custom steps.
    */
    class TypeWrapper :
            public bst_t::void_wrapper
    {
    public:
        virtual int Compare(const T &lhs, const T &rhs) const{
            if(lhs < rhs)
                return -1;
            else if(rhs < lhs)
                return 1;
            return 0;
        }
        virtual T *Copy(const T&o) const{
            return new T(o);
        }
        virtual void Delete(T *o) const{
            delete o;
        }

    private:
        virtual int CompareVoid(const void *const lhs, const void *const rhs) const{
            return Compare(*reinterpret_cast<const T* const>(lhs),
                           *reinterpret_cast<const T* const>(rhs));
        }
        virtual void *CopyVoid(const void *const v) const{
            return reinterpret_cast<void *>(Copy(*reinterpret_cast<const T *const>(v)));
        }
        virtual void DeleteVoid(void *v) const{
            Delete(reinterpret_cast<T*>(v));
        }
    };


    /** Creates an empty BST with the default type wrapper.
        \param cmp A comparison object to determine the sort order of the tree.
        If left 0 it will use the default compare function, the less-than operator.
        The tree will take ownership of this pointer and delete it when finished.
        \sa BST_Type_Wrapper
    */
    inline explicit BinarySearchTree(TypeWrapper *tw = new TypeWrapper)
        :bst_t(tw)
    {}


    /** For iterating depth-first through the BST. */
    class const_iterator :
            public bst_t::const_iterator
    {
    public:
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;

        inline const_iterator(){}
        inline const_iterator(bst_node *n, const Interfaces::IVoidComparer *const vc)
            :bst_t::const_iterator(n, vc)
        {}
        inline const_iterator(const bst_t::const_iterator &o)
            :bst_t::const_iterator(o)
        {}

        /** Dereference the iterator and return a reference to the data. */
        inline const T &operator*() const { return *(reinterpret_cast<const T *const>(current->Data)); }
        /** Dereference the iterator and return a pointer to the data. */
        inline const T *operator->() const { return reinterpret_cast<const T *const>(current->Data); }
    };


    /** Makes a copy of object and puts in the tree.
        If already exists in the tree an exception is thrown.
        \note O(log(N))
    */
    inline const_iterator Add(const T &object){
        return const_iterator(add(reinterpret_cast<const void *const>(&object)), data_access_wrapper);
    }


    /** Removes the object from the tree.  Does nothing if object not in the tree.
        \returns True if item removed
        \note O(log(N))
    */
    inline bool Remove(const T &object){
        return remove(reinterpret_cast<const void *const>(&object));
    }
    inline bool Remove(const const_iterator &iter){
        return remove(iter);
    }


    /** Does a lookup on the given object and returns an iterator to it.
        Returns an invalid iterator equal to end() if not found.
        \note O(log(N))
    */
    inline const_iterator Search(const T &object) const{
        return const_iterator(search(reinterpret_cast<const void *const>(&object)), data_access_wrapper);
    }
    /** Does a lookup with the provided key, which can be a different type than T,
        and returns an iterator to it.  You must provide your own void comparer which knows
        how to interpret the new type and compare it with type T.

        Returns an invalid iterator equal to end() if not found.
        \note O(log(N))
    */
    template<class K>inline const_iterator Search(const K &object, const Interfaces::IVoidComparer *const vc) const{
        return const_iterator(search(reinterpret_cast<const void *const>(&object), vc), data_access_wrapper);
    }

    /** A convenience function which returns if the object exists in the tree.
        \note O(log(N))
    */
    inline bool Contains(const T &object) const{
        return Search(object) != const_iterator();
    }




    /** Returns an iterator starting at the first element in the tree.
        \note O(1)
    */
    inline const_iterator begin() const{
        return const_iterator(first(), data_access_wrapper);
    }
    /** Returns an iterator starting at the end of the tree.  You must decrement it before
        it points to a valid entry.
        \note O(1)
    */
    inline const_iterator end() const{
        return Size() > 0 ? ++const_iterator(last(), data_access_wrapper) : const_iterator();
    }
    /** Returns an iterator starting before the first element in the tree.
        \note O(1)
    */
    inline const_iterator preBegin() const{
        return Size() > 0 ? --const_iterator(first(), data_access_wrapper) : const_iterator();
    }


    /** Returns the least element in the tree.  Throws an exception
        if there are no items in the tree
        \note O(1)
    */
    inline const T &min() const{
        if(Size() == 0)
            THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                       "There are no elements in the BST");
        return *reinterpret_cast<T *>(first()->Data);
    }

    /** Returns the greatest element in the tree.  Throws an exception
        if there are no items in the tree.
        \note O(1)
    */
    inline const T &max() const{
        if(Size() == 0)
            THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                       "There are no elements in the BST");
        return *reinterpret_cast<T *>(last()->Data);
    }

};












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
