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
class Uuid
{
    GBYTE m_data[16];
public:

    /** Constructs a null Uuid */
    inline Uuid(){ Clear(); }

    /** Constructs a Uuid, and optionally generates its value */
    inline explicit Uuid(bool generate_id){
        if(generate_id)
            Generate();
        else
            Clear();
    }

    /** Constructs Uuid from an ascii hex string.
        \sa FromString(), ToString()
    */
    Uuid(const ::GUtil::DataObjects::String &id);

    /** Copy constructor*/
    Uuid(const Uuid &);

    /** Assignment operator */
    inline Uuid &operator = (const Uuid &other){ new(this) Uuid(other); return *this; }

    /** A static constructor of a Uuid */
    inline static Uuid CreateUuid(){ return Uuid(true); }

    /** Returns a Uuid equal to Nil (all zeros).
        \note The default constructor has the same behavior
    */
    inline static Uuid Nil(){ return Uuid(); }

    /** Returns a new Uuid which has been constructed from a string
        \sa ToString()
    */
    inline static Uuid FromString(const ::GUtil::DataObjects::String &s){ return Uuid(s); }

    /** Turns the Uuid object into an ASCII hex string.
        \sa FromString()
    */
    ::GUtil::DataObjects::String ToString() const;

    /** Causes the Uuid to generate a new value. */
    void Generate();

    /** After clearing the Uuid will equal Nil (all bits set to 0) */
    void Clear();

    /** Returns true if the Uuid is equal to Nil (all data set to 0)
        \sa IsNil()
    */
    bool IsNull() const;

    /** Returns true if the Uuid is equal to Nil (all data set to 0)
        \sa IsNull()
    */
    inline bool IsNil() const{ return IsNull(); }

    /** Equality comparator */
    bool operator == (const Uuid &other) const;

    /** Inequality comparator */
    bool operator != (const Uuid &other) const;

};


END_NAMESPACE_GUTIL1;

#endif // UUID_H
