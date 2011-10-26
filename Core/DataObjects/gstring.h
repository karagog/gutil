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

    Also implements a suite of string helper functions, like for string manipulation
    and conversions.

    Use as an alternative to std::string.
*/
class String :
        private Vector<char>
{
public:

    /** Creates a new empty string. */
    inline String() {}

    /** Creates an empty string with the given capacity. */
    inline String(GUINT32 capacity) :Vector<char>(capacity) {}
    /** Creates an empty string with the given capacity. */
    inline String(GINT32 capacity) :Vector<char>(capacity) {}

    /** Creates a new string initialized with the data.
        \param len Specifies the length of the data.  If it is -1 then the string automatically
        finds the terminating null byte (it had better exist in this case!)
    */
    String(const char *d, int len = -1);

    /** Creates a new string initialized with the character repeated the specified number of times. */
    String(char c, int len = 1);

    /** Basically a copy constructor, but for the base type. */
    String(const Vector<char> &s);

    /** Copy constructor. */
    String(const String &s);
    /** Assignment operator. */
    String &operator = (const String &s);

    /** The length of the string. */
    inline GUINT32 Length() const{ return Vector<char>::Length(); }
    inline GUINT32 Capacity() const{ return Vector<char>::Capacity(); }

    /** Returns if the string is null, i.e. has not been initialized. */
    inline bool IsNull() const{ return ConstData() == NULL; }

    /** Returns if it is an empty string, including when it has not been initialized. */
    inline bool IsEmpty() const{ return Length() == 0; }

    /** Clears the string and reclaims the memory. */
    inline void Clear(){ Vector<char>::Clear(); }


    /** Appends the string to this one and returns this. */
    inline String &Append(const String &s){ Insert(s, Length()); return *this; }

    /** Prepends the string to this one and returns this. */
    inline String &Prepend(const String &s){ Insert(s, 0); return *this; }

    /** Inserts the string at the given index. */
    String &Insert(const String &s, GUINT32 indx);
    /** Inserts the raw data at the given index. */
    String &Insert(const char *c, GUINT32 sz, GUINT32 indx);


    /** Returns the left N characters of the string. */
    inline String Left(GUINT32 N) const { return SubString(0, N); }
    /** Returns the right N characters of the string. */
    inline String Right(GUINT32 N) const { return SubString(Length() - N, N); }

    /** Returns the substring starting at the given index and going for the given length. */
    inline String SubString(GUINT32 index, GUINT32 length) const{
        return String(ConstData() + index, length);
    }

    /** Changes all upper case letters to lower case and returns a reference to this. */
    String &ToLower();

    /** Changes all lower case letters to upper case and returns a reference to this. */
    String &ToUpper();

    inline GUINT32 IndexOf(char c, GUINT32 start = 0) const{ return Vector<char>::IndexOf(c, start); }
    GUINT32 IndexOf(const String &, GUINT32 start = 0) const;

    inline GUINT32 LastIndexOf(char c, GUINT32 start = UINT_MAX) const{ return Vector<char>::LastIndexOf(c, start); }
    GUINT32 LastIndexOf(const String &, GUINT32 start = UINT_MAX) const;

    /** Format a string using printf-style strings.  It is a static function, so
        to use it would look like this:

        String s = String::Format("Hello %s, my name is %s", "Suzy", "George")

        \param fmt A format string, which will be passed directly to snprintf, so
        all the formatting rules for that function apply here (see man page for details)

        \return The formatted string.  If the formatting fails for some reason
        then the format string itself is returned.
    */
    static String Format(const char *fmt, ...);

    String Replace(const String &find, const String &replace) const;

    inline Vector<char>::const_iterator begin() const{ return Vector<char>::begin(); }
    inline Vector<char>::iterator begin(){ return Vector<char>::begin(); }
    inline Vector<char>::const_iterator end() const{ return Vector<char>::end(); }
    inline Vector<char>::iterator end(){ return Vector<char>::end(); }
    inline Vector<char>::const_iterator rbegin() const{ return Vector<char>::rbegin(); }
    inline Vector<char>::iterator rbegin(){ return Vector<char>::rbegin(); }
    inline Vector<char>::const_iterator rend() const{ return Vector<char>::rend(); }
    inline Vector<char>::iterator rend(){ return Vector<char>::rend(); }


    /** Comparison operator. */
    bool operator == (const String &s) const;
    /** Comparison operator. */
    bool operator == (const char *s) const;

    /** Comparison operator. */
    inline bool operator != (const String &s) const{ return !(*this == s); }
    /** Comparison operator. */
    bool operator != (const char *s) const{ return !(*this == s); }

    /** Comparison operator. */
    bool operator <  (const String &) const;
    /** Comparison operator. */
    bool operator <= (const String &) const;
    /** Comparison operator. */
    bool operator >  (const String &) const;
    /** Comparison operator. */
    bool operator >= (const String &) const;

    inline String operator + (const String &s) const{ return String(*this).Append(s); }
    inline String &operator += (const String &s){ Append(s); return *this; }

    inline char &operator[] (int i){ return Vector<char>::operator [](i); }
    inline const char &operator[] (int i) const{ return Vector<char>::operator [](i); }
    inline char &operator[] (GUINT32 i){ return Vector<char>::operator [](i); }
    inline const char &operator[] (GUINT32 i) const{ return Vector<char>::operator [](i); }

    inline char &At(GUINT32 i){ return Vector<char>::At(i); }
    inline const char &At(GUINT32 i) const{ return Vector<char>::At(i); }

    // Useful cast operators
    inline operator const char* () const{ return ConstData(); }
    inline operator char* (){ return Data(); }


private:

    inline static int _string_compare(const char *lhs, const char *rhs, GUINT32 len){
        while(len-- != 0)
        {
            if(*lhs == *rhs)
            { lhs++; rhs++; }
            else if(*lhs < *rhs)
                return -1;
            else
                return 1;
        }
        return 0;
    }

};


/** A convenience operator allows you to compare with const char * as a lhs value. */
inline bool operator == (const char *c, const String &s){ return s == c; }

/** A convenience operator that allows you to create strings with the + operator. */
String operator + (const char *c, const String &s);


GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_STRING_H
