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

    // You can define a different equality specification by overriding
    virtual bool compare_equality(const T &lhs, const T &rhs) const{
        return lhs == rhs;
    }

    // Derived classes use this to reject new items for any reason
    virtual void validate_new_item(const T &) const
            throw(Core::ValidationException){}


    CollectionBase(){}
    CollectionBase(const CollectionBase<T> &);
    CollectionBase(const QList<T> &);
    virtual ~CollectionBase(){}

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

    Collection();
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

    bool Contains(const T &) const;
    int IndexOf(const T &, int from = 0) const;


    // The IEquatable interface:
    virtual bool Equals(const Collection<T> &) const;

    // The IClonable interface
    virtual Collection<T> &CloneTo(Collection<T> &) const;

};


GUTIL_END_NAMESPACE


#include "collection.cpp"

#endif // Collection_H
