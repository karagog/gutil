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

#ifdef GUTIL_CORE_QT_ADAPTERS
#include <QByteArray>
#endif // GUTIL_CORE_QT_ADAPTERS

NAMESPACE_GUTIL;


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

    \sa SetGlobalRNG()
*/
template<int NUM_BYTES>

class Id
{
    GBYTE m_data[NUM_BYTES];

    /** A cached null id for fast null comparisons. */
    static const Id s_null;

public:

    /** Returns the number of bytes used in the Id.  This is a fixed constant. */
    static const int Size;

    /** Constructs a null Id, which has all bytes set to 0. */
    Id(){ Clear(); }

    /** Optionally initializes the random ID */
    explicit Id(bool init){
        if(init)    this->Generate();
        else        this->Clear();
    }

    /** Generates a new id.  Use as an alternative to the constructor. */
    static Id<NUM_BYTES> NewId(){ return Id(true); }

    /** Copy constructor */
    Id(const Id<NUM_BYTES> &other){
        memcpy(m_data, other.m_data, sizeof(m_data));
    }

    /** Constructs an Id from an arbitrary byte array.  The array must
     *  be at least NUM_BYTES large, otherwise it will seg fault.
    */
    explicit Id(const GBYTE *d){ memcpy(m_data, d, sizeof(m_data)); }

    /** Constructs an Id from an arbitrary byte array.  The array must
     *  be at least NUM_BYTES large, otherwise it will seg fault.
    */
    explicit Id(const char *d){ memcpy(m_data, d, sizeof(m_data)); }

    /** Constructs an Id from a vector of char.  The vector must
     *  be at least NUM_BYTES large, otherwise it will throw an exception.
    */
    Id(const Vector<char> &v){
        if(v.Length() < sizeof(m_data))
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "Id length too short");
        memcpy(m_data, v.ConstData(), sizeof(m_data));
    }

    /** Assignment operator */
    Id<NUM_BYTES> &operator = (const Id<NUM_BYTES> &other){
        this->~Id(); new(this) Id(other); return *this;
    }

    /** Returns a null id (all bits set to 0). */
    static const Id<NUM_BYTES> &Null(){ return s_null; }

    /** Returns the base of the data array, which has a size of Size(). */
    char const *ConstData() const{ return (char const *)m_data; }


    /** Sets all bytes of the Id to 0. */
    void Clear(){ memset(m_data, 0, sizeof(m_data)); }

    /** Returns true if the Id is null (all bytes are 0). */
    bool IsNull() const{ return 0 == Compare(Null(), *this); }

    /** Generates a random new value for this id. */
    void Generate(){ GUtil::GlobalRNG()->Fill(m_data, sizeof(m_data)); }

    /** Returns an ASCII string representation of the Id with hex digits */
    String ToString16() const{ return String::ToBase16((const char *)m_data, sizeof(m_data)); }

    /** Returns an ASCII string representation of the Id with base 64 digits */
    String ToString64() const{ return String::ToBase64((const char *)m_data, sizeof(m_data)); }

    /** Constructs an Id from an ASCII hex string */
    static Id<NUM_BYTES> FromString16(const String &s){
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
    static Id<NUM_BYTES> FromString64(const String &s){
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
    bool operator == (const Id &other) const{
        return !this->IsNull() && !other.IsNull() && 0 == Compare(*this, other);
    }
    /** Compares two ids for inequality.
        \note Null ids do not equal each other
    */
    bool operator != (const Id &other) const{
        return this->IsNull() || other.IsNull() || 0 != Compare(*this, other);
    }

    /** A compare function that simply compares ID's bitwise.  i.e. Nulls are equal.
        \returns 0 if the ID's bits match.  1 if the lhs is greater, -1 if the rhs is greater.
    */
    static int Compare(const Id &lhs, const Id &rhs){
        return memcmp(lhs.m_data, rhs.m_data, sizeof(Id<NUM_BYTES>::m_data));
    }

    /** Gives you a 4 byte hash of the Id. The hash of the null id is 0.
     *  This is certainly not a secure hash, just a heuristic to generate
     *  a probably unique hash quickly (in O(N) time).
    */
    GUINT32 Hash() const{
        byte bytes[4] = {}; \
        for(int i = 0; i < Size; ++i) \
            bytes[i & 3] ^= m_data[i]; \
        return ((GUINT32)bytes[0] << 24) |
                ((GUINT32)bytes[1] << 16) |
                ((GUINT32)bytes[2] << 8) |
                bytes[3]; \
    }

    /** A less-than operator is defined, to support sorted indexes. */
    bool operator < (const Id &other) const{ return 0 > Compare(*this, other); }
    bool operator > (const Id &other) const{ return 0 < Compare(*this, other); }

    bool operator <= (const Id &other) const{ return 0 >= Compare(*this, other); }
    bool operator >= (const Id &other) const{ return 0 <= Compare(*this, other); }

#ifdef GUTIL_CORE_QT_ADAPTERS
    Id(const QByteArray &b){
        if(b.isEmpty())
            Clear();
        else if(b.length() < (GINT32)sizeof(m_data))
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "Id length too short");
        else
            memcpy(m_data, b.constData(), sizeof(m_data));
    }
    QByteArray ToQByteArray() const{ return QByteArray((const char *)m_data, sizeof(m_data)); }
    explicit operator QByteArray () const { return ToQByteArray(); }
#endif // GUTIL_CORE_QT_ADAPTERS

};


template<int NUM_BYTES>const int Id<NUM_BYTES>::Size = NUM_BYTES;
template<int NUM_BYTES>const Id<NUM_BYTES> Id<NUM_BYTES>::s_null(false);

template<int NUM_BYTES>struct IsMovableType< GUtil::Id<NUM_BYTES> >{ enum{ Value = 1 }; };


END_NAMESPACE_GUTIL;


#ifdef GUTIL_CORE_QT_ADAPTERS



/** A hash function that allows you to use the Id class as a key in a QHash object. */
#define GUTIL_DEFINE_ID_QHASH( NUM_BYTES ) \
    inline GUINT32 qHash(const GUtil::Id<NUM_BYTES> &id){ return id.Hash(); }

#endif

#endif // GUTIL_ID_H
