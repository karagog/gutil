/*Copyright 2010-2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_SHAREDSMARTPOINTER_H
#define GUTIL_SHAREDSMARTPOINTER_H

#include "gutil_macros.h"
#include <atomic>
#include <new>  // For the placement new operator
NAMESPACE_GUTIL;


/** Implements a smart pointer, which behaves like a normal pointer, but also
    manages the memory and deletes it when
    the instance destructs.  You can also Relinquish() the naked pointer if you need to take
    control away from the smart pointer.

    You are not allowed to copy this class, because copying would mean that the pointer
    would be deleted multiple times.

    You can use it with the normal pointer operators.
*/
template<class T>class SmartPointer
{
    GUTIL_DISABLE_COPY(SmartPointer<T>);
    T *ptr;
public:

    /** Initializes the pointer to NULL */
    SmartPointer() :ptr(NULL){}

    /** Initializes the pointer to p. */
    SmartPointer(T *p) :ptr(p){}

    /** Initializes the pointer to p.
     *
     *  \note U must be a type derived from T, and MUST have a virtual destructor to allow
     *  us to delete it.
    */
    template<class U>SmartPointer(U *p) :ptr(dynamic_cast<T *>(p)){}

    /** Assignment operator deletes our current pointer, then looks at another one. */
    template<class U>SmartPointer &operator = (U *p){
        delete ptr;
        new(this) SmartPointer(p);
        return *this;
    }

    /** Delete the pointer when we destruct */
    ~SmartPointer(){ delete ptr; }

    /** Deletes the pointer and sets it to null */
    void Clear(){ delete ptr; ptr = NULL; }

    /** Sets the current pointer, deleting the old one. */
    void Set(T *p){ delete ptr; ptr = p; }

    /** Relinquishes control of the naked pointer, so it will not be deleted when this
        object destructs.  After calling this, the pointer will appear as if newly constructed
        as a NULL pointer.

        \returns The naked pointer that once was controlled by this object.  If this was already
        a null pointer then NULL is returned and nothing bad happens.
    */
    T *Relinquish(){ T *ret(ptr); ptr = NULL; return ret; }

    /** Returns the naked pointer */
    T *Data() const{ return ptr; }

    /** Dereference the pointer */
    T const &operator *() const{ return *ptr; }
    /** Dereference the pointer */
    T &operator *(){ return *ptr; }

    /** Access the pointer's member functions */
    T *operator ->() const{ return ptr; }

    /** Cast operator to the naked pointer.
     *  It is implemented using static_cast, so you can cast to some other pointer types.
    */
    template<class U>operator U *() const{ return static_cast<U *>(ptr); }

    /** Cast operator to a bool, telling you if you're null or not (false => NULL) */
    operator bool() const{ return NULL != ptr; }

    /** Returns true if the pointer is NULL */
    bool IsNull() const{ return !operator bool(); }

    /** Returns if this smart pointer equals the other. */
    bool operator == (const SmartPointer<T> &o) const{ return Data() == o.Data(); }
    /** Returns if this smart pointer does not equal the other. */
    bool operator != (const SmartPointer<T> &o) const{ return Data() != o.Data(); }

};



/** Implements a smart pointer, which manages a pointer and deletes it when
    the instance destructs.  You can also Relinquish() the naked pointer if you need to take
    control away from the smart pointer.

    You are not allowed to copy this class, because copying would mean that the pointer
    would be deleted multiple times.

    You can use it with the normal pointer operators.
*/
template<class T>class SmartArrayPointer
{
    GUTIL_DISABLE_COPY(SmartArrayPointer<T>);
    T *ptr = NULL;
public:

    /** Initializes the pointer to NULL */
    SmartArrayPointer(){}

    /** Initializes the pointer to p, an array of type T. */
    SmartArrayPointer(T *p) :ptr(p){}

    /** Delete the pointer when we destruct */
    ~SmartArrayPointer(){ delete []ptr; }

    /** Deletes the pointer and sets it to null */
    void Clear(){ delete []ptr; ptr = NULL; }

    /** Sets the current array pointer, deleting the old one. */
    void Set(T *p){ delete []ptr; ptr = p; }

    /** Relinquishes control of the naked pointer, so it will not be deleted when this
        object destructs.  After calling this, the pointer will appear as if newly constructed
        as a NULL pointer.

        \returns The naked pointer that once was controlled by this object.  If this was already
        a null pointer then NULL is returned and nothing bad happens.
    */
    T *Relinquish(){ T *ret(ptr); ptr = NULL; return ret; }

    /** Returns the naked pointer */
    T *Data() const{ return ptr; }

    /** Index the array. Returns the item at the index. */
    T const &operator [] (int i) const{ return ptr[i]; }
    /** Index the array. Returns the item at the index. */
    T &operator [] (int i){ return ptr[i]; }

    /** Cast operator to a bool, telling you if you're null or not (false => NULL) */
    operator bool() const{ return NULL != ptr; }

    /** Returns true if the pointer is NULL */
    bool IsNull() const{ return !operator bool(); }

    /** Cast operator to the naked pointer. */
    operator T *() const{ return ptr; }

};




/** Inherit from this class to declare your class able to be used by the shared data
    pointer class.

    It keeps track of references to your class with atomic increment
    decrement classes.

    All functions are protected, because it is not meant to be used by the outside
    world; only by the shared data pointer.
*/
class SharedData
{
    GUTIL_DISABLE_COPY(SharedData);
    template<class T>friend class SharedSmartPointer;
    std::atomic<int> __references{0};
protected:
    SharedData(){}
};



/** Implements an explicitly shared smart pointer.

    It counts the references to a given pointer and deletes it when everybody is
    done referencing it.

    The template class must derive from SharedData
*/
template<class T>class SharedSmartPointer
{
    T *m_data = NULL;
public:

    SharedSmartPointer(){}
    SharedSmartPointer(T *data) :m_data(data){ m_data->__references++; }
    SharedSmartPointer(const SharedSmartPointer<T> &other) :m_data(other.m_data){ m_data->__references++; }

    SharedSmartPointer &operator = (const SharedSmartPointer<T> &other){ return operator = (other.m_data); }
    SharedSmartPointer &operator = (T *data){
        if(m_data && 0 == --(m_data->__references))
            delete m_data;

        if((m_data = data)){
            m_data->__references++;
        }
        return *this;
    }
    ~SharedSmartPointer(){ Clear(); }

    T *Data(){ return m_data; }
    const T *ConstData() const{ return m_data; }

    /** Resets the pointer to 0 and dereferences the object, deleting it if necessary. */
    void Clear(){
        if(m_data && 0 == --(m_data->__references)){
            delete m_data;
            m_data = NULL;
        }
    }

    operator T *(){ return m_data; }
    operator T const *() const{ return m_data; }

    T *operator -> (){ return m_data; }
    const T *operator ->() const{ return m_data; }
    T &operator * (){ return *m_data; }
    const T &operator * () const{ return *m_data; }

    void Detach(){
        if(m_data && 1 < m_data->__references)
        {
            m_data->__references--;
            m_data = new T(*m_data);
            m_data->__references++;
        }
    }

    bool IsNull() const{ return m_data == NULL; }
    operator bool () const{ return m_data; }

    bool operator == (const SharedSmartPointer<T> &o) const{ return ConstData() == o.ConstData(); }
    bool operator != (const SharedSmartPointer<T> &o) const{ return ConstData() != o.ConstData(); }

    /** Cast operator to the naked pointer.
     *  It is implemented using static_cast, so you can cast to some other pointer types.
    */
    template<class U>operator U *() const{ return static_cast<U *>(m_data); }

};



template<class T>class ImplicitlySharedSmartPointer :
        public SharedSmartPointer<T>
{
public:

    ImplicitlySharedSmartPointer(){}
    ImplicitlySharedSmartPointer(T *data) :SharedSmartPointer<T>(data){}
    ImplicitlySharedSmartPointer(const ImplicitlySharedSmartPointer<T> &other) :SharedSmartPointer<T>(other.m_data){}

    T *operator -> (){ SharedSmartPointer<T>::Detach();  return SharedSmartPointer<T>::m_data; }
    const T *operator -> () const{ return SharedSmartPointer<T>::operator ->(); }

    T &operator * (){ SharedSmartPointer<T>::Detach(); return *SharedSmartPointer<T>::m_data; }
    const T &operator * () const{ return SharedSmartPointer<T>::operator *(); }

    T *Data(){ SharedSmartPointer<T>::Detach(); return SharedSmartPointer<T>::m_data; }

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_SHAREDSMARTPOINTER_H
