/*Copyright 2012 George Karagoulis
  
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
NAMESPACE_GUTIL1(Utils);


/** A class that allows you to nullify an ordinary object.

    When the object is null, it will be equal to the GUtil::NullValue value.
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
    inline Nullable<T> &operator = (const T &o){ m_value = o; m_isNull = false; return *this; }
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

    /** Returns a reference to the value */
    inline T *operator ->(){ return &m_value; }
    /** Returns a reference to the value */
    inline T const *operator ->() const{ return &m_value; }
    /** Returns a reference to the value */
    inline T &operator *(){ return m_value; }
    /** Returns a reference to the value */
    inline T const &operator *() const{ return m_value; }

    /** Returns a reference to the value of the nullable */
    inline T &Value(){ return operator *(); }
    /** Returns a reference to the value of the nullable */
    inline T const &Value() const{ return operator *(); }


private:

    T m_value;
    bool m_isNull;

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_NULLABLE_H
