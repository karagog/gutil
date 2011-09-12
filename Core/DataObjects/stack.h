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

#ifndef GUTIL_STACK_H
#define GUTIL_STACK_H

#include "Core/DataObjects/private/stack_p.h"
#include "Core/exception.h"
#include "Core/Interfaces/iclonable.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a FILO stack. */
template<class T>class Stack :
        public stack_t,
        public Interfaces::IClonable< Stack<T> >
{
    GUTIL_DISABLE_COPY(Stack<T>);
public:

    /** The default memory allocator for the stack.  Normally you don't deal with this class*/
    class StackTypeWrapper :
            public stack_t::stack_type_wrapper
    {
    public:
        virtual T* Copy(const T &t) const{
            return new T(t);
        }
        virtual void Delete(T *t) const{
            delete t;
        }
    private:
        void *CopyVoid(const void *const v) const{
            return reinterpret_cast<void *>( Copy(*reinterpret_cast<const T* const>(v)) );
        }
        void DeleteVoid(void *v) const{
            Delete( reinterpret_cast<T*>(v) );
        }
    };

    /** Creates a new stack with the specified type wrapper. */
    inline explicit Stack(StackTypeWrapper *w = new StackTypeWrapper)
        :stack_t(w)
    {}

    /** Pushes an item onto the stack.
        \sa Pop()
        \note O(1)
    */
    void Push(const T &item){
        on_push(item);
        push(reinterpret_cast<const void* const>(&item));
        on_pushed(Top());
    }

    /** Pops the top item from the stack.
        \sa Push()
        \note O(1)
    */
    inline void Pop(){ if(Count()){ iterator i(begin()); Remove(i); } }

    /** Returns the top item on the stack, or 0 if no items in the stack.
        \note O(1)
    */
    inline const T &Top() const{
        return *reinterpret_cast<const T* const>(top());
    }
    /** Returns the top item on the stack, or 0 if no items in the stack.
        \note O(1)
    */
    inline T &Top(){
        return *reinterpret_cast<T*>(top());
    }


    class iterator :
            public forward_node_iterator
    {
    public:
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;

        inline iterator(node_t *n = 0, node_link *parent = 0)
            :forward_node_iterator(n, parent)
        {}
        inline iterator(const forward_node_iterator &o)
            :forward_node_iterator(o.current, o.parent)
        {}

        /** Dereference the iterator and return a reference to the data. */
        inline T &operator*() { return *(reinterpret_cast<T *>(current->Data)); }
        /** Dereference the iterator and return a pointer to the data. */
        inline T *operator->() { return reinterpret_cast<T *>(current->Data); }

        /** Advances the iterator */
        inline iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        inline iterator operator++(int){iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline iterator &operator+=(int n){ while(n-- > 0) advance(); return *this; }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline iterator operator+(int n){ iterator r(*this); while(n-- > 0) r.advance(); return r; }

    };

    class const_iterator :
            public forward_node_iterator
    {
    public:
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;

        inline const_iterator(node_t *const n = 0, const node_link *const parent = 0)
            // Alert: Yes I am casting away the constness, but I am stricly allowing only const
            //  access to it, so I say that's ok, 'cause it means I can still use the forward_node_iterator
            //  for a const_iterator
            :forward_node_iterator(const_cast<node_t *>(n), const_cast<node_link *>(parent))
        {}

        /** Dereference the iterator and return a reference to the data. */
        inline const T &operator*() const { return *(reinterpret_cast<const T *const>(current->Data)); }
        /** Dereference the iterator and return a pointer to the data. */
        inline const T *operator->() const { return reinterpret_cast<const T *const>(current->Data); }

        /** Advances the iterator */
        inline const_iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        inline const_iterator operator++(int){const_iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline const_iterator &operator+=(int n){ while(n-- > 0) advance(); return *this; }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline const_iterator operator+(int n){ const_iterator r(*this); while(n-- > 0) r.advance(); return r; }

    };

    /** Returns an iterator starting at the top of the stack. */
    iterator begin(){
        return iterator(NextNode, this);
    }
    /** Returns an iterator starting at the top of the stack. */
    const_iterator begin() const{
        return const_iterator(NextNode, this);
    }

    /** Returns an invalid iterator that you hit when you iterate to the end of the stack. */
    const_iterator end() const{
        return const_iterator();
    }

    /** Removes the item pointed to by the iterator.

        The virtual functions on_pop() and on_popped() are called, before and after the
        removal.  This is called whenever an item is removed, regardless if it's at the top
        of the stack.

        \note The iterator will remain valid after the removal.  It then points to the next
        element which replaced the one we removed on the stack.
        \note O(N) in the worst case.  It's O(1) if you remove the begin() iterator
        If you are removing a lot from within the stack (aka not the top) then you should
        think about using another class like a linked list.
    */
    void Remove(iterator &iter){
        on_pop(*iter);
        remove(iter);
        on_popped();
    }

    /** Conducts a deep copy of the stack.  Overridden from IClonable.
        \note O(N)
    */
    virtual Stack<T> &CloneTo(Stack<T> &s) const{
        s.Clear();
        _clone_helper(s, NextNode);
        return s;
    }


protected:

    /** Subclasses can take advantage of this to optionally do something when an
        object is pushed onto the stack.  If you throw an exception the operation
        will be safely prevented without memory implications.
        \param The data about to be pushed
    */
    virtual void on_push(const T &){}

    /** Called after an item was pushed onto the stack.
        \param The item that was pushed
    */
    virtual void on_pushed(T &){}

    /** Called before an item is removed.
        \param The data about to be removed
    */
    virtual void on_pop(T &){}

    /** Called after an item is removed. */
    virtual void on_popped(){}


private:

    static void _clone_helper(Stack<T> &s, node_t *n){
        if(n)
        {
            _clone_helper(s, n->NextNode);
            s.Push(*reinterpret_cast<const T *const>(n->Data));
        }
    }

};


GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_STACK_H
