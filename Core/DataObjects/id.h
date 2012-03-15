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

#ifndef GUTIL_ID_H
#define GUTIL_ID_H

#include "gutil_random.h"
#include "gutil_strings.h"
#include <cstring>

NAMESPACE_GUTIL1(DataObjects);


/** Implements a random ID of configurable size.

    The id class provides an implementation of unique ids, which doesn't
    conform to the UUID standard, but gives you more flexibility.  Simply
    put, it is a random ID generator, for which you specify the length
    as a template parameter.

    \tparam NUM_BYTES The size of the Id object, in bytes
*/
template<int NUM_BYTES>

class Id
{
    GBYTE m_data[NUM_BYTES];
public:

    /** Constructs a null Id, which has all bytes set to 0. */
    inline Id(){ Clear(); }

    /** Optionally initializes the random ID */
    inline explicit Id(bool init){
        if(init)    this->Generate();
        else        this->Clear();
    }

    /** Constructs an Id from an ASCII string.

        If the conversion was unsuccessful, the Id will be set to null.
    */
    inline Id(const String &s){
        bool success(true);

        if(s.Length() == (2 * NUM_BYTES)){
            try{
                String cpy( s.FromBase16() );
                memcpy(m_data, cpy.ConstData(), sizeof(m_data));
            }
            catch(...){
                success = false;
            }
        }
        else{
            success = false;
        }

        if(!success)
            Clear();
    }

    /** Copy constructor */
    inline Id(const Id<NUM_BYTES> &other){
        memcpy(m_data, other.m_data, sizeof(m_data));
    }

    /** Assignment operator */
    inline Id<NUM_BYTES> &operator = (const Id<NUM_BYTES> &other){
        new(this) Id(other); return *this;
    }

    /** Sets all bytes of the Id to 0. */
    inline void Clear(){ memset(m_data, 0, sizeof(m_data)); }

    /** Returns true if the Id is null (all bytes are 0). */
    inline bool IsNull() const{
        bool ret(true);
        GBYTE const *cur( m_data ), *end( m_data + NUM_BYTES );
        while(ret && cur != end)
            if(*(cur++) != 0) ret = false;
        return ret;
    }

    inline void Generate(){ ::GUtil::Utils::RandomData::Fill(m_data, sizeof(m_data)); }

    /** Returns an ASCII string representation of the Id with hex digits */
    inline String ToString() const{ return String::ToBase16((const char *)m_data, sizeof(m_data)); }

    /** You can cast an Id seamlessly into a string */
    inline operator String () const{ return ToString(); }

    /** Convenience function constructs an Id from an ASCII hex string */
    inline static Id<NUM_BYTES> FromString(const String &s){ return Id(s); }

    inline bool operator == (const Id &other) const{ return 0 == memcmp(m_data, other.m_data, sizeof(m_data)); }
    inline bool operator != (const Id &other) const{ return 0 != memcmp(m_data, other.m_data, sizeof(m_data)); }

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_ID_H
