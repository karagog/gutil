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

template <typename T> Interfaces::ICollection<T>::~ICollection(){}

template <typename T> void Interfaces::ICollection<T>::Add(const T &value)
{
    int index = _collection.size();
    Insert(value, index);
}

template <typename T> void Interfaces::ICollection<T>::Insert(const T &value, int index)
{
    _collection.insert(index, value);

    onAdd(value, index);
}

template <typename T> void Interfaces::ICollection<T>::Remove(int index)
{
    _collection.removeAt(index);
}

template <typename T> void Interfaces::ICollection<T>::onAdd(const T &, int index){}

template <typename T> void Interfaces::ICollection<T>::onRemove(const T &){}
