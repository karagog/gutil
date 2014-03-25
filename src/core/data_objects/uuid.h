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

NAMESPACE_GUTIL;


/** A Universally Unique Identifier implementation, which conforms to the
    UUID standard.
*/
class Uuid
{
    GBYTE m_data[16];
public:

    /** Constructs a null Uuid */
    Uuid(){ Clear(); }

    /** Constructs a Uuid, and optionally generates its value */
    explicit Uuid(bool generate_id){
        if(generate_id)
            Generate();
        else
            Clear();
    }

    /** Constructs Uuid from an ascii hex string.
        \sa FromString(), ToString()
    */
    Uuid(const GUtil::String &id);

    /** Copy constructor*/
    Uuid(const Uuid &);

    /** Assignment operator */
    Uuid &operator = (const Uuid &other){ new(this) Uuid(other); return *this; }

    /** A static constructor of a Uuid */
    static Uuid CreateUuid(){ return Uuid(true); }

    /** Returns a Uuid equal to Nil (all zeros).
        \note The default constructor has the same behavior
    */
    static Uuid Nil(){ return Uuid(); }

    /** Returns a new Uuid which has been constructed from a string
        \sa ToString()
    */
    static Uuid FromString(const GUtil::String &s){ return Uuid(s); }

    /** Turns the Uuid object into an ASCII hex string.
        \sa FromString()
    */
    GUtil::String ToString() const;

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
    bool IsNil() const{ return IsNull(); }

    /** Equality comparator */
    bool operator == (const Uuid &other) const;

    /** Inequality comparator */
    bool operator != (const Uuid &other) const;

};


END_NAMESPACE_GUTIL;

#endif // UUID_H
