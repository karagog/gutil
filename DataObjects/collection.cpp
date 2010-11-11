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

using namespace GUtil;

template <typename T> DataObjects::Collection<T>::Collection(int size)
{
    Resize(size);
}

template <typename T> DataObjects::Collection<T>::Collection(
        const DataObjects::Collection<T> &o)
{
    o.Clone(*this);
}


template <typename T> DataObjects::Collection<T>::Collection(const QList<T> &o)
{
    _collection = o;
}

template <typename T> DataObjects::Collection<T>::~Collection(){}

template <typename T> T &DataObjects::Collection<T>::Add(const T &value)
{
    FailIfReadOnly();

    int index = _collection.size();
    _collection.append(value);

    return _collection[index];
}

template <typename T> T &DataObjects::Collection<T>::Insert(int index, const T &value)
{
    FailIfReadOnly();

    _collection.insert(index, value);

    return _collection[index];
}

template <typename T> T DataObjects::Collection<T>::Value(int index) const
{
    return _collection.at(index);
}

template <typename T> T &DataObjects::Collection<T>::SetValue(int index, const T &value)
{
    FailIfReadOnly();

    _collection[index] = value;

    return _collection[index];
}

template <typename T> void DataObjects::Collection<T>::Remove(int index)
{
    FailIfReadOnly();

    _collection.removeAt(index);
}

template <typename T> void DataObjects::Collection<T>::Clear()
{
    while(Count() > 0)
        Remove(0);
}

template <typename T> bool DataObjects::Collection<T>::Contains(const T &o) const
{
    return _collection.contain(o);
}

template <typename T> int DataObjects::Collection<T>::IndexOf(const T &o) const
{
    return _collection.IndexOf(o);
}

template <typename T> T &DataObjects::Collection<T>::operator [](int index)
{
    FailIfReadOnly();

    return _collection[index];
}

template <typename T> DataObjects::Collection<T> &
        DataObjects::Collection<T>::operator =(const DataObjects::Collection<T> &o)
{
    return o.CloneOnto(*this);
}

template <typename T> bool DataObjects::Collection<T>::operator ==(
        const DataObjects::Collection<T> &rhs) const
{
    return Equals(rhs);
}

template <typename T> bool DataObjects::Collection<T>::Equals(
        const DataObjects::Collection<T> &rhs) const
{
    return _collection == rhs._collection;
}

template <typename T> int DataObjects::Collection<T>::Count() const
{
    return Size();
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

template <typename T> DataObjects::Collection<T> &DataObjects::Collection<T>::CloneOnto(
        DataObjects::Collection<T> &o) const
{
    o.FailIfReadOnly();

    o._collection = _collection;
    return o;
}
