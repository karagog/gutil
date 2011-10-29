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

#ifndef SHAREDDATAPOINTER_H
#define SHAREDDATAPOINTER_H

#include "Core/DataObjects/shareddata.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements an explicitly shared data pointer.

    It counts the references to a given pointer and deletes it when everybody is
    done referencing it.

    Explicit sharing means that
*/
template<class T>class SharedDataPointer
{
protected:
    T *m_data;
public:

    inline SharedDataPointer() :m_data(0){}
    inline SharedDataPointer(T *data) :m_data(data){ m_data->AddReference(); }
    inline SharedDataPointer(const SharedDataPointer<T> &other) :m_data(other.m_data){ m_data->AddReference(); }

    inline SharedDataPointer &operator = (const SharedDataPointer<T> &other){ operator = (other.m_data); }
    inline SharedDataPointer &operator = (T *data){
        if(m_data && !m_data->RemoveReference())
            delete m_data;

        m_data = data;
        m_data->AddReference();
    }
    inline ~SharedDataPointer(){ if(m_data && !m_data->RemoveReference()) delete m_data; }

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



template<class T>class ImplicitlySharedDataPointer :
        public SharedDataPointer<T>
{
public:

    inline ImplicitlySharedDataPointer(){}
    inline ImplicitlySharedDataPointer(T *data) :SharedDataPointer<T>(data){}
    inline ImplicitlySharedDataPointer(const ImplicitlySharedDataPointer<T> &other) :SharedDataPointer<T>(other.m_data){}

    inline T *operator -> (){ SharedDataPointer<T>::Detach();  return SharedDataPointer<T>::m_data; }
    inline const T *operator -> () const{ return SharedDataPointer<T>::operator ->(); }

    inline T &operator * (){ SharedDataPointer<T>::Detach(); return *SharedDataPointer<T>::m_data; }
    inline const T &operator * () const{ return SharedDataPointer<T>::operator *(); }

    inline T *Data(){ SharedDataPointer<T>::Detach(); return SharedDataPointer<T>::m_data; }

};


GUTIL_END_CORE_NAMESPACE;

#endif // SHAREDDATAPOINTER_H
