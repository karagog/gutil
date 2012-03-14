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

#ifndef GUTIL_UUID_H
#define GUTIL_UUID_H

#include "gutil_strings.h"
#include "gutil_globals.h"

NAMESPACE_GUTIL1(DataObjects);


/** A Universally Unique Identifier implementation.

    This implementation guarantees uniqueness of generated UUIDS.  It allows
    you to configure how many bytes to represent the unique value
    uses 32 random bytes of data (256 random bits), which is larger than most

    \note You can control the way random data is acquired, and subsequently the library
    dependencies.  By default the implementation depends on CryptoPP's OS random
    number generator, which is a really good one.  However, if you don't want
    to depend on the cryptopp dll, you can disable this implementation with the
    GUTIL_NO_CRYPTOPP build switch.  Without cryptopp, it will get random data
    from C's rand() function, which is not nearly as good, but if you don't care
    about the quality of the random data then this may be a fine solution for you.

    \tparam NUM_WORDS The number of random 32-bit words used to represent the unique value.
    This is 1/8 the size of the UUID when converted to an ASCII string.
*/
template<int NUM_WORDS = 8>class Uuid
{
    ::GUtil::DataObjects::String m_data;
public:

    /** Constructs a null Uuid */
    inline Uuid(){}

    /** Constructs a Uuid, and optionally initializes its value*/
    inline explicit Uuid(bool initialize){
        if(initialize) _initialize_data(m_data);
    }

    /** Constructs Uuid from an ascii string.  The string must have been
        created by the ToString() function, or matching this format.

        There is no automatic checking if this value is valid or not.  You can
        call IsValid() after construction to verify that you created a valid
        Uuid.
    */
    inline Uuid(const ::GUtil::DataObjects::String &id)
        :m_data(id.FromBase16()) {}

    /** A static constructor of a Uuid */
    inline static Uuid<NUM_WORDS> CreateUuid(){
        Uuid<NUM_WORDS> ret;  _initialize_data(ret.m_data);  return ret;
    }

    /** Causes the Uuid to initialize its value. */
    inline void Initialize(){ _initialize_data(m_data); }

    /** Turns the Uuid object into an ASCII string */
    inline ::GUtil::DataObjects::String ToString() const{ return m_data.ToBase16(); }

    /** Returns true if the Uuid doesn't have a value */
    inline bool IsNull() const{ return m_data.IsEmpty(); }

    /** Returns true if the Uuid has a value, and it is a valid length */
    inline bool IsValid() const{ return !m_data.IsEmpty() && (NUM_WORDS * 4) == m_data.Length(); }

    /** Equality comparator */
    inline bool operator == (const Uuid<NUM_WORDS> &other) const{ return m_data == other.m_data; }

    /** Inequality comparator */
    inline bool operator != (const Uuid<NUM_WORDS> &other) const{ return m_data != other.m_data; }


private:

    inline static void _initialize_data(::GUtil::DataObjects::String &d){
        d.Resize(NUM_WORDS * 4);
        GINT32 tmp;
        char *cur(d.Data());

        for(int i = 0; i < NUM_WORDS; ++i, cur += 4){
            tmp = GUtil::Rand<GINT32>();
            memcpy(cur, &tmp, 4);
        }
    }

};


END_NAMESPACE_GUTIL1;

#endif // UUID_H
