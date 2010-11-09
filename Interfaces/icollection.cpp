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

//#include "icollection.h"
using namespace GUtil;

template <typename T> Interfaces::ICollection<T>::ICollection(int size)
{
    Resize(size);
}

template <typename T> Interfaces::ICollection<T>::ICollection(
        const Interfaces::ICollection<T> &o)
{
    _copy(*this, o);
}

template <typename T> void Interfaces::ICollection<T>::_copy(Interfaces::ICollection<T> &lhs,
                                       const Interfaces::ICollection<T> &rhs)
{
    lhs.FailIfReadOnly();

    lhs.Clear();

    foreach(T item, rhs._collection)
        lhs._collection.append(item);
}

template <typename T> Interfaces::ICollection<T>::~ICollection(){}

template <typename T> T &Interfaces::ICollection<T>::Add(const T &value)
{
    FailIfReadOnly();

    int index = _collection.size();

    _collection.append(value);
    onAdd(_collection[index]);

    return _collection[index];
}

template <typename T> T &Interfaces::ICollection<T>::Insert(int index, const T &value)
{
    FailIfReadOnly();

    _collection.insert(index, value);

    onAdd((void *)&_collection[index], index);

    return _collection[index];
}

template <typename T> T Interfaces::ICollection<T>::Value(int index) const
{
    return _collection.at(index);
}

template <typename T> T &Interfaces::ICollection<T>::SetValue(int index, const T &value)
{
    FailIfReadOnly();

    _collection[index] = value;

    return _collection[index];
}

template <typename T> void Interfaces::ICollection<T>::Remove(int index)
{
    FailIfReadOnly();

    _collection.removeAt(index);
}

template <typename T> void Interfaces::ICollection<T>::Clear()
{
    while(Count() > 0)
        Remove(0);
}

template <typename T> T &Interfaces::ICollection<T>::operator [](int index)
{
    FailIfReadOnly();

    return _collection[index];
}

template <typename T> Interfaces::ICollection<T> &
        Interfaces::ICollection<T>::operator =(const Interfaces::ICollection<T> &o)
{
    _copy(*this, o);

    return *this;
}

template <typename T> int Interfaces::ICollection<T>::Count() const
{
    return Size();
}

template <typename T> int Interfaces::ICollection<T>::Size() const
{
   return _collection.length();
}

template <typename T> void Interfaces::ICollection<T>::Resize(int len)
{
    FailIfReadOnly();

    if(len > _collection.count())
    {
        while(_collection.count() < len)
            _collection.append(T());
    }
    else if(len < _collection.count())
    {
        while(_collection.count() > len)
            _collection.removeLast();
    }
}

template <typename T> void Interfaces::ICollection<T>::onAdd(T &){}
