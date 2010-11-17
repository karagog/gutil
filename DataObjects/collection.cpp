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

#include <QString>
using namespace GUtil;

template <typename T> DataObjects::Collection<T>::Collection(int size)
    :CollectionBase<T>(size){}

template <typename T> DataObjects::Collection<T>::Collection(
        const DataObjects::Collection<T> &o)
            :CollectionBase<T>(o){}

template <typename T> DataObjects::Collection<T>::Collection(const QList<T> &o)
    :CollectionBase<T>(o){}

template <typename T> DataObjects::Collection<T>::~Collection(){}

template <typename T> T &DataObjects::Collection<T>::Add(const T &value)
{
    return CollectionBase<T>::add_protected(value);
}

template <typename T> T &DataObjects::Collection<T>::Insert(const T &value, int index)
        throw(Core::IndexOutOfRangeException)
{
    return CollectionBase<T>::insert_protected(value, index);
}

template <typename T> const T &DataObjects::Collection<T>::At(int index) const
        throw(Core::IndexOutOfRangeException)
{
    return CollectionBase<T>::at_protected(index);
}

template <typename T> T &DataObjects::Collection<T>::At(int index)
        throw(Core::IndexOutOfRangeException)
{
    return CollectionBase<T>::at_protected(index);
}


template <typename T> void DataObjects::Collection<T>::Remove(int index)
        throw(Core::IndexOutOfRangeException)
{
    CollectionBase<T>::remove_protected(index);
}

template <typename T> void DataObjects::Collection<T>::RemoveOne(const T &i)
{
    CollectionBase<T>::removeOne_protected(i);
}

template <typename T> void DataObjects::Collection<T>::RemoveAll(const T &i)
{
    CollectionBase<T>::removeAll_protected(i);
}

template <typename T> void DataObjects::Collection<T>::Clear()
{
    CollectionBase<T>::clear_protected();
}

template <typename T> bool DataObjects::Collection<T>::Contains(const T &o) const
{
    return CollectionBase<T>::contains_protected(o);
}

template <typename T> int DataObjects::Collection<T>::IndexOf(const T &o, int from) const
{
    return CollectionBase<T>::indexOf_protected(o, from);
}

template <typename T> bool DataObjects::Collection<T>::Equals(
        const DataObjects::Collection<T> &rhs) const
{
    return CollectionBase<T>::Equals(rhs);
}

template <typename T> int DataObjects::Collection<T>::Count() const
{
    return CollectionBase<T>::count_protected();
}

template <typename T> int DataObjects::Collection<T>::Size() const
{
   return CollectionBase<T>::size_protected();
}

template <typename T> void DataObjects::Collection<T>::Resize(int len)
{
    CollectionBase<T>::resize_protected(len);
}

template <typename T> DataObjects::Collection<T> &DataObjects::Collection<T>::CloneTo(
        DataObjects::Collection<T> &o) const
{
    return (DataObjects::Collection<T> &)CollectionBase<T>::CloneTo(o);
}











template <typename T> DataObjects::CollectionBase<T>::CollectionBase(int size)
{
    resize_protected(size);
}

template <typename T> DataObjects::CollectionBase<T>::CollectionBase(
        const DataObjects::CollectionBase<T> &o)
{
    o.CloneTo(*this);
}


template <typename T> DataObjects::CollectionBase<T>::CollectionBase(const QList<T> &o)
{
    _collection = o;
}

template <typename T> DataObjects::CollectionBase<T>::~CollectionBase(){}

template <typename T> T &DataObjects::CollectionBase<T>::add_protected(const T &value)
{
    FailIfReadOnly();

    validate_new_item(value);

    int index = _collection.size();
    _collection.append(value);

    on_add(&_collection[index]);

    return _collection[index];
}

template <typename T> T &DataObjects::CollectionBase<T>::insert_protected(const T &value, int index)
        throw(Core::IndexOutOfRangeException)
{
    FailIfReadOnly();
    _validate_index(index);

    validate_new_item(value);

    _collection.insert(index, value);

    on_add(&_collection[index]);

    return _collection[index];
}

template <typename T> const T &DataObjects::CollectionBase<T>::at_protected(int index) const
        throw(Core::IndexOutOfRangeException)
{
    _validate_index(index);

    return _collection[index];
}

template <typename T> T &DataObjects::CollectionBase<T>::at_protected(int index)
        throw(Core::IndexOutOfRangeException)
{
    _validate_index(index);

    return _collection[index];
}

template <typename T> void DataObjects::CollectionBase<T>::remove_protected(int index)
        throw(Core::IndexOutOfRangeException)
{
    FailIfReadOnly();

    on_remove(&_collection[index]);

    _collection.removeAt(index);
}

template <typename T> void DataObjects::CollectionBase<T>::removeOne_protected(const T &i)
{
    FailIfReadOnly();

    int ind = indexOf_protected(i);
    if(ind >= 0)
        remove_protected(ind);
}

template <typename T> void DataObjects::CollectionBase<T>::removeAll_protected(const T &i)
{
    FailIfReadOnly();

    int ind = 0;
    while((ind = indexOf_protected(i, ind)) >= 0)
        remove_protected(ind);
}

template <typename T> void DataObjects::CollectionBase<T>::clear_protected()
{
    while(count_protected() > 0)
        remove_protected(0);
}

template <typename T> bool DataObjects::CollectionBase<T>::contains_protected(const T &o) const
{
    return indexOf_protected(o) != -1;
}

template <typename T> int DataObjects::CollectionBase<T>::indexOf_protected(const T &o, int from) const
{
    int ret = -1;

    for(int i = from; i < size_protected(); i++)
    {
        if(compare_equality(at_protected(i), o))
        {
            ret = i;
            break;
        }
    }

    return ret;
}

template <typename T> T &DataObjects::CollectionBase<T>::operator [](int index)
        throw(Core::IndexOutOfRangeException)
{
    FailIfReadOnly();
    _validate_index(index);

    return _collection[index];
}

template <typename T> const T &DataObjects::CollectionBase<T>::operator [](int index) const
        throw(Core::IndexOutOfRangeException)
{
    FailIfReadOnly();
    _validate_index(index);

    return _collection[index];
}

template<typename T> void DataObjects::CollectionBase<T>::_validate_index(int index) const
{
    if(index < 0 || index >= count_protected())
        THROW_NEW_GUTIL_EXCEPTION(Core::IndexOutOfRangeException,
                                  QString("Collection only has %1 items in it, and you tried to "
                                          "index the item at %2")
                                  .arg(count_protected()).arg(index).toStdString());
}

template <typename T> bool DataObjects::CollectionBase<T>::Equals(
        const DataObjects::CollectionBase<T> &rhs) const
{
    return _collection == rhs._collection;
}

template <typename T> int DataObjects::CollectionBase<T>::count_protected() const
{
    return _collection.length();
}

template <typename T> int DataObjects::CollectionBase<T>::size_protected() const
{
   return _collection.length();
}

template <typename T> void DataObjects::CollectionBase<T>::resize_protected(int len)
{
    FailIfReadOnly();

    if(len > _collection.count())
    {
        while(_collection.count() < len)
            add_protected(create_blank_item());
    }
    else if(len < _collection.count())
    {
        while(_collection.count() > len)
            remove_protected(_collection.count() - 1);
    }
}

template <typename T> DataObjects::CollectionBase<T> &DataObjects::CollectionBase<T>::CloneTo(
        DataObjects::CollectionBase<T> &o) const
{
    o.FailIfReadOnly();

    o._collection = _collection;
    return o;
}
