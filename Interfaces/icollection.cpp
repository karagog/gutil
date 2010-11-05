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

#include "icollection.h"
using namespace GUtil;

template <typename T> Interfaces::ICollection<T>::ICollection(int size)
{
    Resize(size);
}

template <typename T> Interfaces::ICollection<T>::ICollection(
        const Interfaces::ICollection<T> &o)
{
    *this = o;
}

template <typename T> Interfaces::ICollection<T>::~ICollection(){}

template <typename T> void Interfaces::ICollection<T>::Add(const T &value)
{
    FailIfReadOnly();

    int index = _collection.size();
    Insert(value, index);
}

template <typename T> void Interfaces::ICollection<T>::Insert(int index, const T &value)
{
    FailIfReadOnly();

    _collection.insert(index, 0);
    SetValue(index, value);

    onAdd((void *)&value, index);
}

template <typename T> T Interfaces::ICollection<T>::Value(int index) const
{
    return *_collection.at(index);
}

template <typename T> void Interfaces::ICollection<T>::SetValue(int index, const T &value)
{
    FailIfReadOnly();

    _collection[index] = value;
}

template <typename T> void Interfaces::ICollection<T>::Remove(int index)
{
    FailIfReadOnly();

    _collection.removeAt(index);
}

template <typename T> void Interfaces::ICollection<T>::ClearValues()
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
    FailIfReadOnly();

    ClearValues();

    foreach(T item, o._collection)
        _collection.append(item);

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

template <typename T> void Interfaces::ICollection<T>::onAdd(void *, int){}
