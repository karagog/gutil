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

#ifndef GUTIL_NULLABLE_H
#define GUTIL_NULLABLE_H

#include "Core/globals.h"
NAMESPACE_GUTIL1(DataObjects);


/** A class that allows you to nullify an ordinary object.

    The memory is maintained on the stack, so when you "nullify" an object, its instance
    is only set to the "null value", and the nullable is marked as null.  By doing this we
    avoid repetetive heap allocations/deallocations every time we copy or delete one.

    When comparing nullables, a null value never equals any other value, even another null.

    \note You can still compare the value of the nullable with another value of type T,
    and since the value itself doesn't know anything about "null", it will behave according
    to normal comparison rules.

    When the object of type T is null, its value will be equal to the GUtil::NullValue value.
*/
template<class T>class Nullable
{
public:

    /** The nullable is initially null */
    inline Nullable() :m_value(NullValue()), m_isNull(true){}
    /** The nullable is initialized to the given object; not null */
    inline Nullable(const T &o) :m_value(o), m_isNull(false){}
    /** Conducts a copy of the nullable */
    inline Nullable(const Nullable<T> &o) :m_value(o.m_value), m_isNull(o.m_isNull){}

    /** Assigns the nullable to the object; the result is never null */
    inline Nullable<T> &operator = (const T &o){ SetValue(o); return *this; }
    /** Assigns the nullable to the other one */
    inline Nullable<T> &operator = (const Nullable<T> &o){
        m_value = o.m_value;
        m_isNull = o.m_isNull;
        return *this;
    }

    /** Resets the value to null */
    inline void Clear(){ m_value = NullValue(); m_isNull = true; }

    /** Returns true if the nullable is null */
    inline bool IsNull() const{ return m_isNull; }

    /** This is the value the nullable will take whenever it is null */
    inline static T NullValue(){ return GUtil::NullValue<T>::Value(); }

    /** Returns a reference to the value of the nullable */
    inline T const &Value() const{ return operator *(); }

    /** Sets the nullable to the current value.  The result is never null */
    inline void SetValue(const T &val){ m_value = val; m_isNull = false; }

    /** Returns a const reference to the value */
    inline T const *operator ->() const{ return &m_value; }
    /** Returns a const reference to the value */
    inline T const &operator *() const{ return m_value; }

    /** Returns a reference to the value and lets you modify it.  Afterwards the value is NOT null */
    inline T *operator ->(){ m_isNull = false; return &m_value; }
    /** Returns a reference to the value and lets you modify it.  Afterwards the value is NOT null */
    inline T &operator *(){ m_isNull = false; return m_value; }

    /** Returns true if the nullable equals the given nullable */
    inline bool operator == (const Nullable<T> &o) const{ bool ret(false); if(!IsNull() && !o.IsNull()) ret = m_value == o.m_value; return ret; }
    /** Returns true if the nullable does not equal the given nullable */
    inline bool operator != (const Nullable<T> &o) const{ bool ret(true); if(!IsNull() && !o.IsNull()) ret = m_value != o.m_value; return ret; }

    /** Returns true if the nullable equals the given object */
    inline bool operator == (const T &o) const{ return !IsNull() && Value() == o; }
    /** Returns true if the nullable does not equal the given object */
    inline bool operator != (const T &o) const{ return IsNull() || Value() != o; }

    /** Seamlessly casts us as a const reference to the nullable type */
    inline operator const T &() const{ return m_value; }

    /** Casts us as a TRUE boolean if we are not null, or FALSE if we are null */
    inline operator bool () const{ return !m_isNull; }


private:

    T m_value;
    bool m_isNull;

};


END_NAMESPACE_GUTIL1;


/** Returns true if the nullable equals the given value */
template<class T>
inline bool operator == (const GUtil::DataObjects::Nullable<T> &lhs, const T &rhs){ bool ret(false); if(!lhs.IsNull()) ret = *lhs == rhs; return ret; }
/** Returns false if the nullable equals the given value */
template<class T>
inline bool operator != (const GUtil::DataObjects::Nullable<T> &lhs, const T &rhs){ bool ret(true); if(!lhs.IsNull()) ret = *lhs != rhs; return ret; }

/** Returns true if the given value equals the nullable */
template<class T>
inline bool operator == (const T &lhs, const GUtil::DataObjects::Nullable<T> &rhs){ bool ret(false); if(!rhs.IsNull()) ret = *rhs == lhs; return ret; }
/** Returns false if the given value equals the nullable */
template<class T>
inline bool operator != (const T &lhs, const GUtil::DataObjects::Nullable<T> &rhs){ bool ret(true); if(!rhs.IsNull()) ret = *rhs != lhs; return ret; }



#endif // GUTIL_NULLABLE_H
