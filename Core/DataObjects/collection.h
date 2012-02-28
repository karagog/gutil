/*Copyright 2010-2012 George Karagoulis
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_COLLECTION_H
#define GUTIL_COLLECTION_H

#include "gutil_extendedexception.h"
#include "gutil_list.h"
#include "gutil_vector.h"

NAMESPACE_GUTIL1(DataObjects);


/** A random-access container class, which provides added functionality over
    a simple list.
*/
template <typename T> class Collection
{
public:

    inline explicit Collection(int capacity = 0) :_collection(capacity){}
    inline Collection(const Collection<T> &c) :_collection(c._collection){}
    inline Collection(const DataObjects::List<T> &l) :_collection(l){}

    /** \note It is important that we don't remove items manually,
        to avoid calling the virtual functions in the destructor.
        Just let the list destruct the items for us
    */
    virtual ~Collection(){}

    /** Adds the item to the collection, at the end. */
    inline void Add(const T &value){ Insert(value, Count()); }
    void Insert(const T &value, int index);


    /** A class that will notify the collection when an item is updated. */
    class ItemReference
    {
        friend class Collection;
        Collection *m_collection;
        GUINT32 m_indx;
    public:

        inline const T &Value() const{ return const_cast<const Collection &>(*m_collection)[m_indx]; }
        inline operator const T &() const{ return Value(); }
        inline const T &operator *() const{ return Value(); }
        inline const T *operator ->() const{ return &Value(); }

        inline ItemReference &operator = (const T &v){
            SetValue(v); return *this;
        }


        template<class U>inline bool operator == (const U &v) const{
            return const_cast<const Collection *>(m_collection)->operator [](m_indx) == v;
        }
        template<class U>inline bool operator != (const U &v) const{
            return const_cast<const Collection *>(m_collection)->operator [](m_indx) != v;
        }

        inline void SetValue(const T &v){
            T before(Value());
            m_collection->pre_item_changed(before, v, m_indx);
            m_collection->value_at(m_indx) = v;
            m_collection->post_item_changed(before, m_collection->value_at(m_indx), m_indx);
        }


    protected:

        inline ItemReference(Collection *c, GUINT32 indx)
            :m_collection(c),
              m_indx(indx)
        {}

    };

    inline const T &operator[](GINT32 index) const{ return _collection[index]; }
    inline ItemReference operator[](GINT32 index){ return ItemReference(this, index); }
    inline const T &operator[](GUINT32 index) const{ return _collection[index]; }
    inline ItemReference operator[](GUINT32 index){ return ItemReference(this, index); }

    inline const T &At(GINT32 index) const{ return _collection.At(index); }
    inline ItemReference At(GINT32 index){
        if(index < 0 || index >= Count()) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return ItemReference(this, index);
    }
    inline const T &At(GUINT32 index) const{ return _collection.At(index); }
    inline ItemReference At(GUINT32 index){
        if(index < 0 || index >= Count()) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return ItemReference(this, index);
    }

    inline void Remove(int index){
        if(0 > index || index >= Count()) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

        pre_remove(_collection[index], index);
        T cpy(operator[](index));
        _collection.RemoveAt(index);
        post_remove(cpy, index);
    }

    inline void RemoveOne(const T &i){
        int ind = IndexOf(i);
        if(-1 != ind)
            Remove(ind);
    }

    void RemoveAll(const T &i){
        int ind = 0;
        while(-1 != (ind = IndexOf(i, ind)))
            Remove(ind);
    }

    inline void Clear(){
        int indx(Count());
        while(indx-- > 0)
            Remove(indx);
    }

    inline int Count() const{ return _collection.Count(); }
    inline int Size() const{ return Count(); }

    inline bool Contains(const T &i) const{ return -1 != IndexOf(i); }
    int IndexOf(const T &i, int from = 0) const;


    inline Collection<T> &operator << (const T &o){
        Add(o); return *this;
    }



    /** An iterator for the collection, which allows you to access const member functions,
        and allows you to assign to the elements, but you cannot call non-const member functions.
    */
    class iterator :
            public List<T>::iterator
    {
        friend class Collection;
        Collection *m_collection;
    public:

        inline iterator(const iterator &iter)
            :List<T>::iterator(iter),
              m_collection(iter.m_collection)
        {}

        /** You can access const member functions with this. */
        inline T const &operator *() const{ return List<T>::iterator::operator *(); }
        /** You can access const member functions with this. */
        inline T const *operator ->() const{ return List<T>::iterator::operator ->(); }

        /** The only way to modify a value is by assigning to it. */
        void SetValue(const T &v){
            ItemReference(m_collection, List<T>::iterator::Index()).SetValue(v);
	}


    protected:

        inline iterator(const typename List<T>::iterator &iter, Collection<T> *c)
            :List<T>::iterator(iter),
              m_collection(c)
        {}

        // Hide these functions, because they could modify an item
        inline T &operator *() { return List<T>::iterator::operator *(); }
        inline T *operator ->() { return List<T>::iterator::operator ->(); }

    };

    class const_iterator :
            public List<T>::const_iterator
    {
        friend class Collection;
    public:

        inline const_iterator(const const_iterator &iter)
            :List<T>::const_iterator(iter)
        {}

    protected:

        inline const_iterator(const typename List<T>::const_iterator &iter)
            :List<T>::const_iterator(iter)
        {}

    };

    inline iterator begin(){ return iterator(_collection.begin(), this); }
    inline iterator end(){ return iterator(_collection.end(), this); }
    inline const_iterator begin() const{ return _collection.begin(); }
    inline const_iterator end() const{ return _collection.end(); }

    inline iterator rbegin(){ return iterator(_collection.rbegin(), this); }
    inline iterator rend(){ return iterator(_collection.rend(), this); }
    inline const_iterator rbegin() const{ return _collection.rbegin(); }
    inline const_iterator rend() const{ return _collection.rend(); }


protected:

    /** Called before an item is added.  You may change or validate this item.
        If you throw an exception, the adding will be prevented
    */
    virtual void pre_add(T &item, int indx){}

    /** Called after an item is added.  You may change this item, but validation
        must be done in the pre_add function.
    */
    virtual void post_add(T &item, int indx){}

    /** Called before an item is removed.  You may change or validate this item's removal.
        If you throw an exception, the removal will be prevented
    */
    virtual void pre_remove(T &item, int indx){}

    /** Called after an item is removed.  You are only given a copy of
        the value of the object, as it once existed in the collection,
        but now this item has been removed, so we only have a copy.
    */
    virtual void post_remove(const T &removed_item, int indx){}

    /** Called before an item is changed.  You may validate the change
        and prevent it by throwing an exception.
    */
    virtual void pre_item_changed(const T &value_before, const T &new_value, int indx){}

    /** Called after an item is changed.  You may update the new value if you wish.
    */
    virtual void post_item_changed(const T &value_before, T &new_value, int indx){}

    /** Friends use this accessor function to get at the values without using
        an ItemReference.
    */
    inline T &value_at(GINT32 indx){ return _collection[indx]; }


private:

    DataObjects::List<T> _collection;

};



/** This version of the collection class requires you to implement a default constructor,
    so that the collection is resizeable.
*/
template <class T> class ResizableCollection :
        public Collection<T>
{
public:

    inline explicit ResizableCollection(int capacity = 0)
        :Collection<T>(capacity) {}

    inline ResizableCollection(const DataObjects::List<T> &v)
        : Collection<T>(v){}

    inline ResizableCollection(const ResizableCollection<T> &v)
        : Collection<T>(v){}


    inline void Resize(int size)
    {
        if(size < 0)
            return;

        if(size < Collection<T>::Count())
        {
            while(Collection<T>::Count() > size)
                Collection<T>::Remove(Collection<T>::Count() - 1);
        }
        else
        {
            while(Collection<T>::Count() < size)
                Collection<T>::Add( T() );
        }
    }

};




template<class T> class ObservableCollection :
        public ResizableCollection<T>
{
public:

    inline explicit ObservableCollection(int capacity) :ResizableCollection<T>(capacity){}
    inline ObservableCollection(const ObservableCollection &o)
        :ResizableCollection<T>(o){}


    /** Classes who wish to observe the collection
        may derive from this and add themselves as an observer
    */
    class Observer
    {
    public:

        virtual void pre_add(T &item, int indx){}
        virtual void post_add(T &item, int indx){}

        virtual void pre_remove(T &item, int indx){}
        virtual void post_remove(const T &removed_item, int indx){}

        virtual void pre_item_changed(const T &value_before, const T &new_value, int indx){}
        virtual void post_item_changed(const T &value_before, T &new_value, int indx){}

    };

    inline void AddCollectionObserver(Observer *o){ m_observers.PushBack(o); }
    inline void RemoveCollectionObserver(Observer *o){ m_observers.RemoveAll(o); }


protected:

    virtual void pre_add(T &item, int indx){
        for(typename Vector<Observer *>::iterator iter(m_observers.begin()); iter != m_observers.end(); ++iter)
            (*iter)->pre_add(item, indx);
    }
    virtual void post_add(T &item, int indx){
        for(typename Vector<Observer *>::iterator iter(m_observers.begin()); iter != m_observers.end(); ++iter)
            (*iter)->post_add(item, indx);
    }
    virtual void pre_remove(T &item, int indx){
        for(typename Vector<Observer *>::iterator iter(m_observers.begin()); iter != m_observers.end(); ++iter)
            (*iter)->pre_remove(item, indx);
    }
    virtual void post_remove(const T &removed_item, int indx){
        for(typename Vector<Observer *>::iterator iter(m_observers.begin()); iter != m_observers.end(); ++iter)
            (*iter)->post_remove(removed_item, indx);
    }
    virtual void pre_item_changed(const T &value_before, const T &new_value, int indx){
        for(typename Vector<Observer *>::iterator iter(m_observers.begin()); iter != m_observers.end(); ++iter)
            (*iter)->pre_item_changed(value_before, new_value, indx);
    }
    virtual void post_item_changed(const T &value_before, T &new_value, int indx){
        for(typename Vector<Observer *>::iterator iter(m_observers.begin()); iter != m_observers.end(); ++iter)
            (*iter)->post_item_changed(value_before, new_value, indx);
    }


private:

    Vector<Observer *> m_observers;

};









template <typename T>
void Collection<T>::Insert(const T &value, int index)
{
    if(index < 0 || index > Count())
        THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

    T new_item(value);

    pre_add(new_item, index);
    _collection.Insert(new_item, index);
    post_add(_collection[index], index);
}

template <typename T> int Collection<T>::IndexOf(const T &o, int from) const
{
    int ret = -1;

    for(int i = from; i < Count(); i++)
    {
        if(operator[](i) == o)
        {
            ret = i;
            break;
        }
    }

    return ret;
}








END_NAMESPACE_GUTIL1;


#endif // GUTIL_COLLECTION_H

