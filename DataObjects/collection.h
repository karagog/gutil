/*Copyright 2010 George Karagoulis
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef COLLECTION_H
#define COLLECTION_H

#include "Core/exception.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include "Core/Interfaces/iequatable.h"
#include "Core/Interfaces/iclonable.h"
#include "gutil_macros.h"
#include <QList>


GUTIL_BEGIN_NAMESPACE(DataObjects);


template <typename T> class CollectionBase :
        public Core::Interfaces::IReadOnlyObject,
        protected Core::Interfaces::IEquatable< CollectionBase<T> >,
        protected Core::Interfaces::IClonable< CollectionBase<T> >
{
protected:

    // Derived classes can do things to the added/removed object by overriding
    virtual void on_add(T *) const{}
    virtual void on_remove(T *) const{}

    // You can define a different equality specification by overriding
    virtual bool compare_equality(const T &lhs, const T &rhs) const{
        return lhs == rhs;
    }

    // Derived classes use this method to instantiate blank items for the resize method
    virtual T create_blank_item(){
        THROW_NEW_GUTIL_EXCEPTION(Core::NotImplementedException,
                                  "Derived class must override this method!");
    }

    // Derived classes use this to reject new items for any reason
    virtual void validate_new_item(const T &) const
            throw(Core::ValidationException){}


    CollectionBase(int size = 0);
    CollectionBase(const CollectionBase<T> &);
    CollectionBase(const QList<T> &);
    virtual ~CollectionBase();

    T &add_protected(const T &value);
    T &insert_protected(const T &value, int index)
            throw(Core::IndexOutOfRangeException);

    const T &at_protected(int index) const
            throw(Core::IndexOutOfRangeException);
    T &at_protected(int index)
            throw(Core::IndexOutOfRangeException);

    void remove_protected(int index)
            throw(Core::IndexOutOfRangeException);
    void removeOne_protected(const T &);
    void removeAll_protected(const T &);
    void clear_protected();

    int count_protected() const;
    int size_protected() const;
    void resize_protected(int);

    bool contains_protected(const T &) const;
    int indexOf_protected(const T &, int from = 0) const;


    // The IEquatable interface:
    virtual bool Equals(const CollectionBase<T> &) const;

    // The IClonable interface
    virtual CollectionBase<T> &CloneTo(CollectionBase<T> &) const;


private:

    QList<T> _collection;

    void _validate_index(int) const;

};



template <typename T> class Collection : public CollectionBase<T>
{
public:

    Collection(int size = 0);
    Collection(const Collection<T> &);
    Collection(const QList<T> &);
    virtual ~Collection();


    T &Add(const T &value);
    T &Insert(const T &value, int index)
            throw(Core::IndexOutOfRangeException);

    const T &At(int index) const
            throw(Core::IndexOutOfRangeException);
    T &At(int index)
            throw(Core::IndexOutOfRangeException);

    T &operator [](int index)
            throw(Core::IndexOutOfRangeException);

    const T &operator [](int index) const
            throw(Core::IndexOutOfRangeException);

    void Remove(int index)
            throw(Core::IndexOutOfRangeException);
    void RemoveOne(const T &);
    void RemoveAll(const T &);
    void Clear();

    int Count() const;
    int Size() const;
    void Resize(int);

    bool Contains(const T &) const;
    int IndexOf(const T &, int from = 0) const;


    // The IEquatable interface:
    virtual bool Equals(const Collection<T> &) const;

    // The IClonable interface
    virtual Collection<T> &CloneTo(Collection<T> &) const;

};




// Like the other collection, but it stores a pointer instead, allowing you to store
//   classes derived from the base class T.
// Warning: If you resize it without initializing the new values, you will get a
//   null reference exception
template <typename T> class PointerCollection : public CollectionBase<T *>
{
public:

    PointerCollection(int size = 0);
    PointerCollection(const CollectionBase<T *> &);
    virtual ~PointerCollection(){}


    T &Add(const T &value);
    T &Insert(const T &value, int index)
            throw(Core::IndexOutOfRangeException);

    const T &At(int index) const
            throw(Core::IndexOutOfRangeException);
    T &At(int index)
            throw(Core::IndexOutOfRangeException);

    const T &operator [](int index) const
            throw(Core::IndexOutOfRangeException);
    T &operator [](int index)
            throw(Core::IndexOutOfRangeException);

    void Remove(int index)
            throw(Core::IndexOutOfRangeException);
    void RemoveOne(const T &);
    void RemoveAll(const T &);
    void Clear();

    int Count() const;
    int Size() const;
    void Resize(int);

    bool Contains(const T &) const;
    int IndexOf(const T &, int from = 0) const;


    // The IClonable interface
    virtual PointerCollection<T> &CloneTo(PointerCollection<T> &) const;


protected:

    // This function is called on each item in the CloneTo function
    virtual T *clone_item(T * const &i) const{ return new T(*i); }


    virtual T *create_blank_item(){ return 0; }

    virtual bool compare_equality(T * const &,
                                  T * const &) const;

    // For convenience, derived classes can still implement their own custom
    //  comparison logic by overriding this instead of compare_equality
    virtual bool compare_equality_dereferenced(const T &lhs, const T &rhs) const{
        return lhs == rhs;
    }


    virtual void validate_new_item(T * const &i) const
            throw(Core::ValidationException)
    {
        if(i)
            validate_new_item_dereferenced(*i);
    }

    virtual void validate_new_item_dereferenced(const T &) const{}


    virtual void on_add(T *t) const{ if(t) on_add_dereferenced(*t); }
    virtual void on_add_dereferenced(T &) const{}

    virtual void on_remove(T **t) const{
        if(*t)
        {
            on_remove_dereferenced(**t);
            delete *t;
        }
    }
    virtual void on_remove_dereferenced(T &) const{}


private:

    T *_check_pointer(T * const &) const;

};


GUTIL_END_NAMESPACE


#include "collection.cpp"

#endif // Collection_H
