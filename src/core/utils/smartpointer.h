/*Copyright 2010-2013 George Karagoulis

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

#include "gutil_atomic.h"
#include "gutil_macros.h"
NAMESPACE_GUTIL1(Utils);


/** Implements a smart pointer, which manages a pointer and deletes it when
    the instance destructs.

    You are not allowed to copy this class, because copying would mean that the pointer
    would be deleted multiple times.

    You can use it with the normal pointer operators.
*/
template<class T>class SmartPointer
{
    T *ptr;
    GUTIL_DISABLE_COPY(SmartPointer<T>);
public:

    /** Initializes the pointer to NULL */
    inline SmartPointer() :ptr(0){}

    /** Initializes the pointer to p
        \note p may be of type T, or a type derived from T.  In this case it must have a virtual destructor
    */
    template<class U>inline SmartPointer(U *p) :ptr(dynamic_cast<T *>(p)){}

    /** Assignment operator deletes our current pointer, then looks at another one. */
    template<class U>inline SmartPointer &operator = (U *p){
        delete ptr;
        new(this) SmartPointer(p);
        return *this;
    }

    /** Delete the pointer when we destruct */
    inline ~SmartPointer(){ delete ptr; }

    /** Deletes the pointer and sets it to null */
    inline void Clear(){ delete ptr; ptr = 0; }

    /** Returns the naked pointer */
    inline T *Data() const{ return ptr; }

    /** Dereference the pointer */
    inline T const &operator *() const{ return *ptr; }
    /** Dereference the pointer */
    inline T &operator *(){ return *ptr; }

    /** Access the pointer's member functions */
    inline T const *operator ->() const{ return ptr; }
    /** Access the pointer's member functions */
    inline T *operator ->(){ return ptr; }

    /** Cast operator to the naked pointer */
    inline operator T *() const{ return ptr; }
    /** Cast operator to a bool, telling you if you're null or not (false => NULL) */
    inline operator bool() const{ return ptr; }

    /** Returns true if the pointer is NULL */
    inline bool IsNull() const{ return !operator bool(); }

    /** Returns if this smart pointer equals the other. */
    inline bool operator == (const SmartPointer<T> &o) const{ return Data() == o.Data(); }
    /** Returns if this smart pointer does not equal the other. */
    inline bool operator != (const SmartPointer<T> &o) const{ return Data() != o.Data(); }

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
    GUTIL_DISABLE_COPY(SharedData)
    template<class T>friend class SharedSmartPointer;

    /** Counts another reference to the shared data */
    inline bool AddReference(){ return m_references.Increment(); }
    /** Removes another reference to the shared data */
    inline bool RemoveReference(){ return m_references.Decrement(); }

    /** Returns the current reference count to the shared data */
    inline GINT32 ReferenceCount() const{ return m_references; }

    Utils::AtomicInt m_references;


protected:

    inline SharedData(){}

};



/** Implements an explicitly shared smart pointer.

    It counts the references to a given pointer and deletes it when everybody is
    done referencing it.

    The template class must derive from SharedData
*/
template<class T>class SharedSmartPointer
{
    T *m_data;
public:

    inline SharedSmartPointer() :m_data(0){}
    inline SharedSmartPointer(T *data) :m_data(data){ m_data->AddReference(); }
    inline SharedSmartPointer(const SharedSmartPointer<T> &other) :m_data(other.m_data){ m_data->AddReference(); }

    inline SharedSmartPointer &operator = (const SharedSmartPointer<T> &other){ return operator = (other.m_data); }
    inline SharedSmartPointer &operator = (T *data){
        if(m_data && !m_data->RemoveReference())
            delete m_data;

        m_data = data;
        m_data->AddReference();
        return *this;
    }
    inline ~SharedSmartPointer(){ Clear(); }

    inline T *Data(){ return m_data; }
    inline const T *ConstData() const{ return m_data; }

    /** Resets the pointer to 0 and dereferences the object, deleting it if necessary. */
    inline void Clear(){
        if(m_data && !m_data->RemoveReference()){
            delete m_data;
            m_data = 0;
        }
    }

    inline operator T *(){ return m_data; }
    inline operator T const *() const{ return m_data; }

    inline T *operator -> (){ return m_data; }
    inline const T *operator ->() const{ return m_data; }
    inline T &operator * (){ return *m_data; }
    inline const T &operator * () const{ return *m_data; }

    inline void Detach(){
        if(m_data && m_data->ReferenceCount() > 1)
        {
            m_data->RemoveReference();
            m_data = new T(*m_data);
            m_data->AddReference();
        }
    }

    inline bool IsNull() const{ return m_data == NULL; }
    inline operator bool () const{ return m_data; }

    inline bool operator == (const SharedSmartPointer<T> &o) const{ return ConstData() == o.ConstData(); }
    inline bool operator != (const SharedSmartPointer<T> &o) const{ return ConstData() != o.ConstData(); }

};



template<class T>class ImplicitlySharedSmartPointer :
        public SharedSmartPointer<T>
{
public:

    inline ImplicitlySharedSmartPointer(){}
    inline ImplicitlySharedSmartPointer(T *data) :SharedSmartPointer<T>(data){}
    inline ImplicitlySharedSmartPointer(const ImplicitlySharedSmartPointer<T> &other) :SharedSmartPointer<T>(other.m_data){}

    inline T *operator -> (){ SharedSmartPointer<T>::Detach();  return SharedSmartPointer<T>::m_data; }
    inline const T *operator -> () const{ return SharedSmartPointer<T>::operator ->(); }

    inline T &operator * (){ SharedSmartPointer<T>::Detach(); return *SharedSmartPointer<T>::m_data; }
    inline const T &operator * () const{ return SharedSmartPointer<T>::operator *(); }

    inline T *Data(){ SharedSmartPointer<T>::Detach(); return SharedSmartPointer<T>::m_data; }

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_SHAREDSMARTPOINTER_H
