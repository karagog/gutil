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
#include <cstdarg>

GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a contiguous string of characters, whose memory is managed automatically.

    Also implements a suite of string helper functions, like for string manipulation
    and conversions.

    The requirements for this class are as follows:
    * Always maintains enough capacity for at least the amount of characters,
        plus the terminating null charater
    * Always maintains the terminating null, and at any given time throughout a
        string's life the character at index 'Length()' should be a '\0', given
        that the string has been initialized (not IsNull())


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


    /** Removes 'len' characters starting at index 'indx'. */
    String &Remove(GUINT32 indx, GUINT32 len);


    /** Returns the left N characters of the string. */
    inline String Left(GUINT32 N) const { return SubString(0, N); }
    /** Returns the right N characters of the string. */
    inline String Right(GUINT32 N) const { return SubString(Length() - N, N); }

    /** Returns the substring starting at the given index and going for the given length. */
    inline String SubString(GUINT32 index, GUINT32 length) const{
        return String(ConstData() + index, length);
    }


    enum
    {
        /** Where the lower case letters start in ascii. */
        LowercaseOffset = 0x61,

        /** Where the upper case letters start in ascii. */
        UppercaseOffset  = 0x41
    };

    /** Changes all upper case letters to lower case and returns a reference to this. */
    String &ToLower();

    /** Changes all lower case letters to upper case and returns a reference to this. */
    String &ToUpper();

    /** Returns the lower case version of the letter. */
    inline static char ToLower(char c){
        if(UppercaseOffset <= c && c < (UppercaseOffset + 26))
            c = c - (UppercaseOffset - LowercaseOffset);
        return c;
    }
    /** Returns the upper case version of the letter. */
    inline static char ToUpper(char c){
        if(LowercaseOffset <= c && c < (LowercaseOffset + 26))
            c = c - (LowercaseOffset - UppercaseOffset);
        return c;
    }

    inline GUINT32 IndexOf(char c, GUINT32 start = 0) const{ return Vector<char>::IndexOf(c, start); }
    inline GUINT32 IndexOf(const String &s, GUINT32 start = 0) const{
        return IndexOf(s.ConstData(), start, s.Length());
    }
    GUINT32 IndexOf(const char *, GUINT32 start = 0, GUINT32 string_length = UINT_MAX) const;

    inline GUINT32 LastIndexOf(char c, GUINT32 start = UINT_MAX) const{ return Vector<char>::LastIndexOf(c, start); }
    inline GUINT32 LastIndexOf(const String &s, GUINT32 start = UINT_MAX) const{
        return LastIndexOf(s.ConstData(), start, s.Length());
    }
    GUINT32 LastIndexOf(const char *, GUINT32 start = UINT_MAX, GUINT32 string_length = UINT_MAX) const;

    /** Format a string using printf-style strings.  It is a static function, so
        to use it would look like this:

        String s = String::Format("Hello %s, my name is %s", "Suzy", "George")

        \param fmt A format string, which will be passed directly to snprintf, so
        all the formatting rules for that function apply here (see man page for details)

        \return The formatted string.  If the formatting fails for some reason
        then the format string itself is returned.
    */
    inline static String Format(const char *fmt, ...){
        va_list args;
        va_start(args, fmt);
        return vFormat(fmt, args);
    }

    /** The non-static version of Format(), in which the current string is the
        format string.
    */
    inline String Format(...) const{
        va_list args;
        va_start(args, 0);
        return vFormat(ConstData(), args);
    }

    /** The same as Format() except it takes a va_list as an argument.
        \note This calls va_end for you
    */
    static String vFormat(const char *fmt, va_list);

    /** The non-static version of vFormat(), which takes a va_list as an argument.
        \note This calls va_end for you
    */
    inline String vFormat(va_list args) const{ return vFormat(ConstData(), args); }


    /** Returns a copy of this string, which has been parsed linearly to replace
        any instance of 'find' with 'replace'
        \param find A null-terminated string to search for in this string
        \param replace A null-terminated string with which to replace 'find'
        \param case_sensitive Whether or not to search with case sensitivity. Replace
        will always retain the same case it had.
    */
    String Replace(const char *find, const char *replace, bool case_sensitive = true) const;

    bool ToBool(bool *ok = 0) const;
    GINT32 ToInt(bool *ok = 0) const;
    GFLOAT32 ToFloat(bool *ok = 0) const;

    inline static String FromBool(bool b){ return b ? "1" : "0"; }
    inline static String FromInt(GINT32 i){ return String::Format("%d", i); }
    inline static String FromFloat(GFLOAT32 d){ return String::Format("%#f", d); }


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

    inline static int _string_compare_from_end(const char *lhs, const char *rhs, GUINT32 len){
        while(len-- != 0)
        {
            if(*lhs == *rhs)
            { lhs--; rhs--; }
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
