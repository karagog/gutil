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

#include "gutil_rng.h"
#include "gutil_strings.h"
#include <cstring>

NAMESPACE_GUTIL1(DataObjects);


/** Implements a random ID of configurable size.

    The id class provides an implementation of unique ids, which doesn't
    conform to the UUID standard, but gives you more flexibility.  Simply
    put, it is a random ID generator, for which you specify the length
    as a template parameter.

    It should be noted that a Null Id never equals any other Id, even another
    null Id.  You should compensate for this in your logic.

    \tparam NUM_BYTES The size of the Id object, in bytes

    \note If you are using the CryptoPP random number generator, you must initialize
    the RNG before using this class.

    \sa InitializeRNG()
*/
template<int NUM_BYTES>

class Id
{
    GBYTE m_data[NUM_BYTES];

    /** A cached null id for fast null comparisons. */
    static const Id s_null;


public:

    /** A convenience function that initializes the RNG, in case it needs to be.

        You must call this before using any Id object, because the RNG is used
        to generate its value.

        You can call this function as many times as you want, but it must be called
        at least once before you start using Id's!
    */
    inline static void InitializeRNG(){ ::GUtil::Utils::RNG::Initialize(); }

    /** Constructs a null Id, which has all bytes set to 0. */
    inline Id(){ Clear(); }

    /** Optionally initializes the random ID */
    inline explicit Id(bool init){
        if(init)    this->Generate();
        else        this->Clear();
    }

    /** Generates a new id.  Use as an alternative to the constructor. */
    static inline Id<NUM_BYTES> NewId(){ return Id(true); }

    /** Copy constructor */
    inline Id(const Id<NUM_BYTES> &other){
        memcpy(m_data, other.m_data, sizeof(m_data));
    }

    /** Constructs an Id from an arbitrary byte array.  The array must
     *  be at least NUM_BYTES large, otherwise it will seg fault.
    */
    inline explicit Id(const GBYTE *d){
        memcpy(m_data, d, sizeof(m_data));
    }

    /** Assignment operator */
    inline Id<NUM_BYTES> &operator = (const Id<NUM_BYTES> &other){
        this->~Id(); new(this) Id(other); return *this;
    }

    /** Returns a null id (all bits set to 0). */
    static inline const Id<NUM_BYTES> &Null(){ return s_null; }

    /** Returns the number of bytes used in the Id.  This is a fixed constant. */
    static inline int Size(){ return NUM_BYTES; }

    /** Returns the base of the data array, which has a size of Size(). */
    inline char const *ConstData() const{ return (char const *)m_data; }


    /** Sets all bytes of the Id to 0. */
    inline void Clear(){ memset(m_data, 0, sizeof(m_data)); }

    /** Returns true if the Id is null (all bytes are 0). */
    inline bool IsNull() const{ return 0 == Compare(Null(), *this); }

    /** Generates a random new value for this id. */
    inline void Generate(){ ::GUtil::Utils::RNG::Fill(m_data, sizeof(m_data)); }

    /** Returns an ASCII string representation of the Id with hex digits */
    inline String ToString16() const{ return String::ToBase16((const char *)m_data, sizeof(m_data)); }

    /** Returns an ASCII string representation of the Id with base 64 digits */
    inline String ToString64() const{ return String::ToBase64((const char *)m_data, sizeof(m_data)); }

    /** Constructs an Id from an ASCII hex string */
    static inline Id<NUM_BYTES> FromString16(const String &s){
        Id<NUM_BYTES> ret;
        if(s.Length() == (2 * NUM_BYTES)){
            try{
                String cpy( s.FromBase16() );
                memcpy(ret.m_data, cpy.ConstData(), sizeof(ret.m_data));
            }
            catch(...){}
        }
        return ret;
    }

    /** Constructs an Id from an ASCII base-64 string */
    static inline Id<NUM_BYTES> FromString64(const String &s){
        Id<NUM_BYTES> ret;
        try{
            String cpy( s.FromBase64() );
            if(sizeof(ret.m_data) == cpy.Length())
                memcpy(ret.m_data, cpy.ConstData(), sizeof(ret.m_data));
        }
        catch(...){}
        return ret;
    }

    /** Compares two ids for equality.
        \note Null ids do not equal each other
    */
    inline bool operator == (const Id &other) const{
        return !this->IsNull() && !other.IsNull() && 0 == Compare(*this, other);
    }
    /** Compares two ids for inequality.
        \note Null ids do not equal each other
    */
    inline bool operator != (const Id &other) const{
        return this->IsNull() || other.IsNull() || 0 != Compare(*this, other);
    }

    /** A compare function that simply compares ID's bitwise.  i.e. Nulls are equal.
        \returns 0 if the ID's bits match.  1 if the lhs is greater, -1 if the rhs is greater.
    */
    static inline int Compare(const Id &lhs, const Id &rhs){
        return memcmp(lhs.m_data, rhs.m_data, sizeof(Id<NUM_BYTES>::m_data));
    }

    /** A less-than operator is defined, to support sorted indexes. */
    inline bool operator < (const Id &other) const{ return 0 > Compare(*this, other); }
    inline bool operator > (const Id &other) const{ return 0 < Compare(*this, other); }

    inline bool operator <= (const Id &other) const{ return 0 >= Compare(*this, other); }
    inline bool operator >= (const Id &other) const{ return 0 <= Compare(*this, other); }

};


template<int NUM_BYTES> const Id<NUM_BYTES> Id<NUM_BYTES>::s_null(false);


END_NAMESPACE_GUTIL1;


namespace GUtil{
    template<int NUM_BYTES>struct IsMovableType< ::GUtil::DataObjects::Id<NUM_BYTES> >{ enum{ Value = 1 }; };
}

#endif // GUTIL_ID_H
