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
{
    Resize(size);
}

template <typename T> DataObjects::Collection<T>::Collection(
        const DataObjects::Collection<T> &o)
{
    o.CloneTo(*this);
}


template <typename T> DataObjects::Collection<T>::Collection(const QList<T> &o)
{
    _collection = o;
}

template <typename T> DataObjects::Collection<T>::~Collection(){}

template <typename T> T &DataObjects::Collection<T>::Add(const T &value)
{
    FailIfReadOnly();

    validate_new_item(value);

    int index = _collection.size();
    _collection.append(value);

    on_add(&_collection[index]);

    return _collection[index];
}

template <typename T> T &DataObjects::Collection<T>::Insert(int index, const T &value)
        throw(Core::IndexOutOfRangeException)
{
    FailIfReadOnly();

    if(index < 0 || index >= Count())
        THROW_NEW_GUTIL_EXCEPTION(Core::IndexOutOfRangeException,
                                  QString("Collection only has %1 items in it, and you tried to "
                                          "index the item at %2")
                                  .arg(Count()).arg(index).toStdString());

    validate_new_item(value);

    _collection.insert(index, value);

    on_add(&_collection[index]);

    return _collection[index];
}

template <typename T> T DataObjects::Collection<T>::Value(int index) const
        throw(Core::IndexOutOfRangeException)
{
    if(index < 0 || index >= Count())
        THROW_NEW_GUTIL_EXCEPTION(Core::IndexOutOfRangeException,
                                  QString("Collection only has %1 items in it, and you tried to "
                                          "index the item at %2")
                                  .arg(Count()).arg(index).toStdString());

    return _collection.at(index);
}

template <typename T> T &DataObjects::Collection<T>::SetValue(int index, const T &value)
        throw(Core::IndexOutOfRangeException)
{
    FailIfReadOnly();

    if(index < 0 || index >= Count())
        THROW_NEW_GUTIL_EXCEPTION(Core::IndexOutOfRangeException,
                                  QString("Collection only has %1 items in it, and you tried to "
                                          "index the item at %2")
                                  .arg(Count()).arg(index).toStdString());

    _collection[index] = value;

    return _collection[index];
}

template <typename T> void DataObjects::Collection<T>::Remove(int index)
        throw(Core::IndexOutOfRangeException)
{
    FailIfReadOnly();

    on_remove(&_collection[index]);

    _collection.removeAt(index);
}

template <typename T> void DataObjects::Collection<T>::RemoveOne(const T &i)
{
    FailIfReadOnly();

    int ind = IndexOf(i);
    if(ind >= 0)
        Remove(ind);
}

template <typename T> void DataObjects::Collection<T>::RemoveAll(const T &i)
{
    FailIfReadOnly();

    int ind = 0;
    while((ind = IndexOf(i, ind)) >= 0)
        Remove(ind);
}

template <typename T> void DataObjects::Collection<T>::Clear()
{
    while(Count() > 0)
        Remove(0);
}

template <typename T> bool DataObjects::Collection<T>::Contains(const T &o) const
{
    return IndexOf(o) != -1;
}

template <typename T> int DataObjects::Collection<T>::IndexOf(const T &o) const
{
    int ret = -1;

    for(int i = 0; i < Size(); i++)
    {
        if(compare_equality(Value(i), o))
        {
            ret = i;
            break;
        }
    }

    return ret;
}

template <typename T> T &DataObjects::Collection<T>::operator [](int index)
        throw(Core::IndexOutOfRangeException)
{
    FailIfReadOnly();

    if(index < 0 || index >= Count())
        THROW_NEW_GUTIL_EXCEPTION(Core::IndexOutOfRangeException,
                                  QString("Collection only has %1 items in it, and you tried to "
                                          "index the item at %2")
                                  .arg(Count()).arg(index).toStdString());

    return _collection[index];
}

template <typename T> bool DataObjects::Collection<T>::Equals(
        const DataObjects::Collection<T> &rhs) const
{
    return _collection == rhs._collection;
}

template <typename T> int DataObjects::Collection<T>::Count() const
{
    return _collection.length();
}

template <typename T> int DataObjects::Collection<T>::Size() const
{
   return _collection.length();
}

template <typename T> void DataObjects::Collection<T>::Resize(int len)
{
    FailIfReadOnly();

    if(len > _collection.count())
    {
        while(_collection.count() < len)
            Add(create_blank_item());
    }
    else if(len < _collection.count())
    {
        while(_collection.count() > len)
            Remove(_collection.count() - 1);
    }
}

template <typename T> DataObjects::Collection<T> &DataObjects::Collection<T>::CloneTo(
        DataObjects::Collection<T> &o) const
{
    o.FailIfReadOnly();

    o._collection = _collection;
    return o;
}
