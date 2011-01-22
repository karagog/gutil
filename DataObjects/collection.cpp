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

template <typename T> GUtil::DataObjects::Collection<T>::Collection()
    :CollectionBase<T>(){}

template <typename T> GUtil::DataObjects::Collection<T>::Collection(
        const DataObjects::Collection<T> &o)
            :CollectionBase<T>(o){}

template <typename T> GUtil::DataObjects::Collection<T>::Collection(const QList<T> &o)
    :CollectionBase<T>(o){}

template <typename T> GUtil::DataObjects::Collection<T>::~Collection(){}

template <typename T> T &GUtil::DataObjects::Collection<T>::Add(const T &value)
{
    return CollectionBase<T>::add_protected(value);
}

template <typename T> T &GUtil::DataObjects::Collection<T>::Insert(const T &value, int index)
        throw(Core::IndexOutOfRangeException)
{
    return CollectionBase<T>::insert_protected(value, index);
}

template <typename T> const T &GUtil::DataObjects::Collection<T>::At(int index) const
        throw(Core::IndexOutOfRangeException)
{
    return CollectionBase<T>::at_protected(index);
}

template <typename T> T &GUtil::DataObjects::Collection<T>::At(int index)
        throw(Core::IndexOutOfRangeException)
{
    return CollectionBase<T>::at_protected(index);
}

template <typename T> T &GUtil::DataObjects::Collection<T>::operator [](int index)
        throw(Core::IndexOutOfRangeException)
{
    return CollectionBase<T>::at_protected(index);
}

template <typename T> const T &GUtil::DataObjects::Collection<T>::operator [](int index) const
        throw(Core::IndexOutOfRangeException)
{
    return CollectionBase<T>::at_protected(index);
}


template <typename T> void GUtil::DataObjects::Collection<T>::Remove(int index)
        throw(Core::IndexOutOfRangeException)
{
    CollectionBase<T>::remove_protected(index);
}

template <typename T> void GUtil::DataObjects::Collection<T>::RemoveOne(const T &i)
{
    CollectionBase<T>::removeOne_protected(i);
}

template <typename T> void GUtil::DataObjects::Collection<T>::RemoveAll(const T &i)
{
    CollectionBase<T>::removeAll_protected(i);
}

template <typename T> void GUtil::DataObjects::Collection<T>::Clear()
{
    CollectionBase<T>::clear_protected();
}

template <typename T> bool GUtil::DataObjects::Collection<T>::Contains(const T &o) const
{
    return CollectionBase<T>::contains_protected(o);
}

template <typename T> int GUtil::DataObjects::Collection<T>::IndexOf(const T &o, int from) const
{
    return CollectionBase<T>::indexOf_protected(o, from);
}

template <typename T> bool GUtil::DataObjects::Collection<T>::Equals(
        const DataObjects::Collection<T> &rhs) const
{
    return CollectionBase<T>::Equals(rhs);
}

template <typename T> int GUtil::DataObjects::Collection<T>::Count() const
{
    return CollectionBase<T>::count_protected();
}

template <typename T> int GUtil::DataObjects::Collection<T>::Size() const
{
   return CollectionBase<T>::size_protected();
}

template <typename T> GUtil::DataObjects::Collection<T> &GUtil::DataObjects::Collection<T>::CloneTo(
        DataObjects::Collection<T> &o) const
{
    return (GUtil::DataObjects::Collection<T> &)CollectionBase<T>::CloneTo(o);
}











template <typename T> GUtil::DataObjects::CollectionBase<T>::CollectionBase(
        const DataObjects::CollectionBase<T> &o)
            :IReadOnlyObject(o)
{
    o.CloneTo(*this);
}


template <typename T> GUtil::DataObjects::CollectionBase<T>::CollectionBase(const QList<T> &o)
    :_collection(o)
{}

template <typename T> T &GUtil::DataObjects::CollectionBase<T>::add_protected(const T &value)
{
    FailIfReadOnly();

    validate_new_item(value);

    int index = _collection.size();
    _collection.append(value);

    on_add(index);

    return _collection[index];
}

template <typename T> T &GUtil::DataObjects::CollectionBase<T>::insert_protected(const T &value, int index)
        throw(Core::IndexOutOfRangeException)
{
    FailIfReadOnly();
    if(index < 0 || index > this->count_protected())
        throw Core::IndexOutOfRangeException(QString("Cannot insert at index %1")
                                             .arg(index).toStdString());

    validate_new_item(value);

    _collection.insert(index, value);

    on_add(index);

    return _collection[index];
}

template <typename T> const T &GUtil::DataObjects::CollectionBase<T>::at_protected(int index) const
        throw(Core::IndexOutOfRangeException)
{
    _validate_index(index);

    return _collection[index];
}

template <typename T> T &GUtil::DataObjects::CollectionBase<T>::at_protected(int index)
        throw(Core::IndexOutOfRangeException)
{
    _validate_index(index);

    return _collection[index];
}

template <typename T> void GUtil::DataObjects::CollectionBase<T>::remove_protected(int index)
        throw(Core::IndexOutOfRangeException)
{
    FailIfReadOnly();

    on_remove(index);

    _collection.removeAt(index);
}

template <typename T> void GUtil::DataObjects::CollectionBase<T>::removeOne_protected(const T &i)
{
    int ind = indexOf_protected(i);
    if(ind >= 0)
        remove_protected(ind);
}

template <typename T> void GUtil::DataObjects::CollectionBase<T>::removeAll_protected(const T &i)
{
    int ind = 0;
    while((ind = indexOf_protected(i, ind)) >= 0)
        remove_protected(ind);
}

template <typename T> void GUtil::DataObjects::CollectionBase<T>::clear_protected()
{
    while(count_protected() > 0)
        remove_protected(0);
}

template <typename T> bool GUtil::DataObjects::CollectionBase<T>::contains_protected(const T &o) const
{
    return indexOf_protected(o) != -1;
}

template <typename T> int GUtil::DataObjects::CollectionBase<T>::indexOf_protected(const T &o, int from) const
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

template<typename T> void GUtil::DataObjects::CollectionBase<T>::_validate_index(int index) const
{
    if(index < 0 || index >= count_protected())
        THROW_NEW_GUTIL_EXCEPTION2(Core::IndexOutOfRangeException,
                                  QString("Collection only has %1 items in it, and you tried to "
                                          "index the item at %2")
                                  .arg(count_protected()).arg(index).toStdString());
}

template <typename T> bool GUtil::DataObjects::CollectionBase<T>::Equals(
        const GUtil::DataObjects::CollectionBase<T> &rhs) const
{
    bool ret = _collection == rhs._collection;

    for(int i = 0; ret && i < count_protected(); i++)
        if(!compare_equality(at_protected(i), rhs.at_protected(i)))
            ret = false;

    return ret;
}

template <typename T> int GUtil::DataObjects::CollectionBase<T>::count_protected() const
{
    return _collection.length();
}

template <typename T> int GUtil::DataObjects::CollectionBase<T>::size_protected() const
{
   return _collection.length();
}

template <typename T> GUtil::DataObjects::CollectionBase<T> &
GUtil::DataObjects::CollectionBase<T>::CloneTo(
    GUtil::DataObjects::CollectionBase<T> &o) const
{
    o.FailIfReadOnly();

    o._collection = _collection;
    return o;
}
