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

#ifndef GUTIL_STRING_H
#define GUTIL_STRING_H
#include "Core/DataObjects/vector.h"

GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a contiguous string of characters, whose memory is managed automatically.

    Use as an alternative to std::string.
*/
class String :
        public Vector<char>
{
public:

    /** Creates a new empty string. */
    String();

    /** Creates an empty string with the given capacity. */
    String(int capacity);

    /** Creates a new string initialized with the data.
        \param len Specifies the length of the data.  If it is -1 then the string automatically
        finds the terminating null byte (it had better exist in this case!)
    */
    String(const char *, int len = -1);

    /** Creates a new string initialized with the character repeated the specified number of times. */
    String(const char c, int len = 1);

    /** Basically a copy constructor, but for the base type. */
    inline String(const Vector<char> &s) :Vector<char>(s){}


    /** Appends the string to this one and returns this. */
    inline String &Append(const String &s){ Vector<char>::Insert(s, Length()); return *this; }
    /** Prepends the string to this one and returns this. */
    inline String &Prepend(const String &s){ Vector<char>::Insert(s, 0); return *this; }

    /** Inserts the string at the given index. */
    String &Insert(const String &, int indx);

    int IndexOf(const String &, GUINT32 start = 0) const;
    int LastIndexOf(const String &, GUINT32 start = UINT_MAX) const;

    /** Comparison operator. */
    bool operator == (const String &) const;
    /** Comparison operator. */
    bool operator <  (const String &) const;
    /** Comparison operator. */
    bool operator <= (const String &) const;
    /** Comparison operator. */
    bool operator >  (const String &) const;
    /** Comparison operator. */
    bool operator >= (const String &) const;

    String operator + (const String &) const;
    String &operator += (const String &) const;

    // Useful cast operators
    inline operator const char* () const{ return ConstData(); }
    inline operator char* (){ return Data(); }

};


GUTIL_END_CORE_NAMESPACE;


static GUtil::Core::DataObjects::String operator + (const char *c, const GUtil::Core::DataObjects::String &s);


#endif // GUTIL_STRING_H
