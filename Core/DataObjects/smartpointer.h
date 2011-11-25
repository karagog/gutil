/*Copyright 2011 George Karagoulis

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

#include "Core/DataObjects/shareddata.h"
#include "gutil_macros.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


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

    inline SmartPointer() :ptr(NULL){}
    inline SmartPointer(T *p) :ptr(p){}

    inline ~SmartPointer(){
        if(ptr) delete ptr;
    }

    inline SmartPointer &operator = (T *p){
        if(ptr) delete ptr;
        ptr = p;
    }

    inline T &operator *() const{ return *ptr; }
    inline T *operator ->() const{ return ptr; }

    inline operator T *() const{ return ptr; }
    inline operator bool() const{ return ptr; }

};


/** Implements an explicitly shared smart pointer.

    It counts the references to a given pointer and deletes it when everybody is
    done referencing it.
*/
template<class T>class SharedSmartPointer
{
protected:
    T *m_data;
public:

    inline SharedSmartPointer() :m_data(0){}
    inline SharedSmartPointer(T *data) :m_data(data){ m_data->AddReference(); }
    inline SharedSmartPointer(const SharedSmartPointer<T> &other) :m_data(other.m_data){ m_data->AddReference(); }

    inline SharedSmartPointer &operator = (const SharedSmartPointer<T> &other){ operator = (other.m_data); }
    inline SharedSmartPointer &operator = (T *data){
        if(m_data && !m_data->RemoveReference())
            delete m_data;

        m_data = data;
        m_data->AddReference();
    }
    inline ~SharedSmartPointer(){ if(m_data && !m_data->RemoveReference()) delete m_data; }

    inline T *Data(){ return m_data; }
    inline const T *ConstData() const{ return m_data; }

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


GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_SHAREDSMARTPOINTER_H
