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

    /** The number of bytes we're capable of holding. */
    inline GUINT32 Capacity() const{ return Vector<char>::Capacity(); }

    /** The number of UTF-8 characters (may differ from the actual byte length of the string). */
    GUINT32 LengthUTF8() const;

    /** Returns if the string is null, i.e. has not been initialized. */
    inline bool IsNull() const{ return ConstData() == NULL; }

    /** Returns if it is an empty string, including when it has not been initialized. */
    inline bool IsEmpty() const{ return Length() == 0; }

    /** Clears the string and reclaims the memory. */
    inline void Clear(){ Vector<char>::Clear(); }


    /** Appends the string to this one and returns this. */
    inline String &Append(const String &s){ Insert(s, Length()); return *this; }

    /** Appends the character the specified number of times. */
    inline String &Append(char c, GUINT32 cnt = 1){
        if(cnt == 1)
            Insert(&c, 1, Length());
        else
            Insert(String(c, cnt), Length());
        return *this;
    }

    /** Prepends the string to this one and returns this. */
    inline String &Prepend(const String &s){ Insert(s, 0); return *this; }

    /** Prepends the character the specified number of times. */
    inline String &Prepend(char c, GUINT32 cnt = 1){
        if(cnt == 1)
            Insert(&c, 1, 0);
        else
            Insert(String(c, cnt), 0);
        return *this;
    }


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

    /** Returns a copy of this string with all upper case letters changed to lower case. */
    String ToLower() const;

    /** Returns a copy of this string with all lower case letters changed to upper case. */
    String ToUpper() const;

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

    /** Remove the last n letters from the string. */
    inline String &Chop(GUINT32 n){
        if(n <= Length()) Remove(Length() - n, n);
        return *this;
    }


    /** Removes whitespace characters from the front and back of this string.
        \return A reference to this string after trimming.
    */
    String &Trim();

    /** Removes whitespace characters from the front and back of a copy of this string.
        \return A copy of this string after trimming.
    */
    inline String Trimmed() const{ String ret(*this); return ret.Trim(); }

    /** Returns true if the character is considered whitespace.
        \note This is used by the Trim() function
    */
    inline static bool IsWhitespace(char c){
        return (0 <= c && c <= 0x20) || c == 0x7F;
    }

    /** Splits the string using the given character delimiter.
        \param separator A single character delimiter
    */
    inline Vector<String> Split(char separator, bool keep_empty_parts = true) const{
        char c[] = {separator, '\0'};
        return Split(c, keep_empty_parts);
    }

    /** Splits the string using the given string delimiter.
        \param separator A null-terminated string delimiter
    */
    Vector<String> Split(const char *separator, bool keep_empty_parts = true) const;

    inline static String Join(const Vector<String> &v, char separator){ return Join(v, &separator, 1); }
    inline static String Join(const Vector<String> &v, const String &separator){ return Join(v, separator.ConstData(), separator.Length()); }
    static String Join(const Vector<String> &, const char *separator, GUINT32 len = UINT_MAX);



    /** Returns true if the character is an ASCII character (non-extended). */
    inline static bool IsValidAscii(char c){ return c >= 0;  }

    inline static bool IsValidUTF8StartByte(GINT8 b){ return !IsValidUTF8ContinuationByte(b); }
    inline static bool IsValidUTF8ContinuationByte(GINT8 b){
        return GINT8(0x80) == (b & GINT8(0xC0));
    }
    static bool IsValidUTF8Sequence(const char *start, const char *end);

    /** Returns the theoretical byte length of the multibyte character
        starting with start_byte
    */
    inline static GINT8 MultiByteLength(char start_byte){
        GINT8 msb( FSB8(~(start_byte)) );
        return msb == 7 ? 1 : 7 - msb;
    }

    static GUINT32 UnicodeValue(const char *multibyte_start, GINT8 multibyte_length);

    /** Returns a copy of this string, in which only valid UTF-8 characters are retained.

        In the event that an error in the UTF-8 encoding is encountered, the
        unrecognized bytes will be stripped from the string.

        \note If the string is valid ascii, then it will be the same when you call ToUTF8()
    */
    String ToUTF8() const;

    /** Returns whether the string is in a valid UTF-8 encoding.
        You can optionally pass a pointer to an integer, which would tell you how many
        bytes are invalid.
        \note The function is optimized so if you pass a NULL pointer (the default)
        it won't parse the rest of the string once it finds the first bad byte.
    */
    bool IsValidUTF8(GINT32 *bad_bytes = NULL) const;

    /** Returns the base64 version of this string.
        \note The resulting string will be 4/3 as large.
    */
    inline String ToBase64() const{ return ToBase64(ConstData(), Length()); }

    /** Returns the base64 version of this string. */
    static String ToBase64(const char *, GUINT32 len = UINT_MAX);

    /** Returns the decoded version of this base64 string.
        If the string is not actually a base64 string then the behavior is undefined.
        \note The resulting string will be 3/4 as large
    */
    inline String FromBase64() const{ return FromBase64(ConstData(), Length()); }

    /** Returns the decoded version of the base64 string. */
    static String FromBase64(const char *, GUINT32 len = UINT_MAX);


    /** Given a base 64 character, it returns the number it represents. */
    static char Base64a2i(char);

    /** Given a number, it returns the base 64 character for it. */
    static char Base64i2a(char);


    /** Returns the base16 version of this string.
        \note The resulting string will be 2 times as large.
    */
    inline String ToBase16() const{ return ToBase16(ConstData(), Length()); }

    /** Returns the base16 version of the given string. */
    static String ToBase16(const char *, GUINT32 len = UINT_MAX);

    /** Returns the decoded version of this base16 string.
        \note The resulting string will be half as large.
    */
    inline String FromBase16() const{ return FromBase16(ConstData(), Length()); }

    /** Returns the decoded version of the base16 string. */
    static String FromBase16(const char *, GUINT32 len = UINT_MAX);

    /** Given a hex digit as an ascii character, this returns the 4-bit hex value. */
    static char CharToHex(char);

    /** Given a 4-bit hex value, this returns the ascii character used in the
        base16 numbering system.
        \note Always returns upper case characters
    */
    static char HexToChar(char);


    inline Vector<char>::const_iterator begin() const{ return Vector<char>::begin(); }
    inline Vector<char>::iterator begin(){ return Vector<char>::begin(); }
    inline Vector<char>::const_iterator end() const{ return Vector<char>::end(); }
    inline Vector<char>::iterator end(){ return Vector<char>::end(); }
    inline Vector<char>::const_iterator rbegin() const{ return Vector<char>::rbegin(); }
    inline Vector<char>::iterator rbegin(){ return Vector<char>::rbegin(); }
    inline Vector<char>::const_iterator rend() const{ return Vector<char>::rend(); }
    inline Vector<char>::iterator rend(){ return Vector<char>::rend(); }


    /** An iterator that helps you with parsing the String object.

        Strings are stored internally as UTF-8 characters, so this gives you
        all the functions necessary for interacting with the string one UTF-8 character
        at a time.
    */
    class UTF8Iterator
    {
        char *m_begin;
        char *m_end;
        char *m_cur;
    public:

        inline UTF8Iterator(char *begin, char *end, char *cur)
            :m_begin(begin), m_end(end), m_cur(cur)
        {}

        inline bool IsValidStartByte() const{ return String::IsValidUTF8StartByte(*m_cur); }
        inline bool IsValidContinuationByte() const{ return String::IsValidUTF8ContinuationByte(*m_cur); }

        /** Returns true if we're positioned on a valid UTF-8 character sequence,
            but doesn't validate the Unicode character value itself.
        */
        bool IsValidUTF8Sequence() const{ return String::IsValidUTF8Sequence(m_cur, m_cur + ByteLength()); }

        /** Returns the Unicode codepoint of the current character, or -1 if it is
            on an invalid character.
        */
        inline GUINT32 UnicodeValue() const{ return String::UnicodeValue(m_cur, ByteLength()); }

        /** Returns the number of bytes represented by this character.
            \note If currently positioned on an invalid UTF-8 character,
            then the return of this function is undefined.
            \sa IsValidStartByte()
        */
        inline GINT8 ByteLength() const{ GINT8 msb( FSB8(~(*m_cur)) );
                                         return msb == 7 ? 1 : 7 - msb;
                                        }

        /** Returns the Unicode codepoint of the current character, or -1 if it is
            on an invalid character.
        */
        inline GINT32 operator *() const{ return UnicodeValue(); }

        /** Prefix increment, advances the iterator and returns it. */
        inline UTF8Iterator &operator ++(){ _advance(); return *this; }
        /** Postfix increment, advances the iterator but returns the iterator it was before. */
        inline UTF8Iterator operator ++(int){ UTF8Iterator ret(*this); _advance(); return ret; }

        /** Prefix decrement, retreats the iterator and returns it. */
        inline UTF8Iterator &operator --(){ _retreat(); return *this; }
        /** Postfix decrement, retreats the iterator but returns the iterator it was before. */
        inline UTF8Iterator operator --(int){ UTF8Iterator ret(*this); _retreat(); return ret; }

        /** Returns true if we are positioned on a valid index.
            \note This says nothing about the character itself being a valid UTF-8 character.
        */
        inline operator bool () const{ return m_begin <= m_cur && m_cur < m_end && *m_cur != '\0'; }


    private:

        inline void _advance(){
            if(IsValidUTF8Sequence())
                m_cur += ByteLength();
            else
                ++m_cur;
        }
        inline void _retreat(){
            --m_cur;
            while(String::IsValidUTF8ContinuationByte(*m_cur) && m_cur >= m_begin)
                --m_cur;
        }

    };

    /** A const iterator that helps you with parsing the String object.

        Strings are stored internally as UTF-8 characters, so this gives you
        all the functions necessary for interacting with the string one UTF-8 character
        at a time.
    */
    class UTF8ConstIterator
    {
        const char *m_begin;
        const char *m_end;
        const char *m_cur;
    public:

        inline UTF8ConstIterator(const char *begin, const char *end, const char *cur)
            :m_begin(begin), m_end(end), m_cur(cur)
        {}

        inline bool IsValidStartByte() const{ return String::IsValidUTF8StartByte(*m_cur); }


        inline bool IsValidContinuationByte() const{ return String::IsValidUTF8ContinuationByte(*m_cur); }


        /** Returns true if we're positioned on a valid UTF-8 character sequence,
            but doesn't validate the Unicode character value itself.
        */
        inline bool IsValidUTF8Sequence() const{ return String::IsValidUTF8Sequence(m_cur, m_cur + ByteLength()); }

        /** Returns the Unicode codepoint of the current character, or -1 if it is
            on an invalid character.
        */
        inline GUINT32 UnicodeValue() const{ return String::UnicodeValue(m_cur, ByteLength()); }

        /** Returns the number of bytes represented by this character.
            \note If currently positioned on an invalid UTF-8 character,
            then the return of this function is undefined.
            \sa IsValidStartByte()
        */
        inline GINT8 ByteLength() const{ return String::MultiByteLength(*m_cur); }

        /** Returns the Unicode codepoint of the current character, or -1 if it is
            on an invalid character.
        */
        inline GINT32 operator *() const{ return UnicodeValue(); }

        /** Prefix increment, advances the iterator and returns it. */
        inline UTF8ConstIterator &operator ++(){ _advance(); return *this; }
        /** Postfix increment, advances the iterator but returns the iterator it was before. */
        inline UTF8ConstIterator operator ++(int){ UTF8ConstIterator ret(*this); _advance(); return ret; }

        /** Prefix decrement, retreats the iterator and returns it. */
        inline UTF8ConstIterator &operator --(){ _retreat(); return *this; }
        /** Postfix decrement, retreats the iterator but returns the iterator it was before. */
        inline UTF8ConstIterator operator --(int){ UTF8ConstIterator ret(*this); _retreat(); return ret; }

        /** Returns true if we are positioned on a valid index.
            \note This says nothing about the character itself being a valid UTF-8 character.
        */
        inline operator bool () const{ return m_begin <= m_cur && m_cur < m_end && *m_cur != '\0'; }


    private:

        inline void _advance(){
            if(IsValidUTF8Sequence())
                m_cur += ByteLength();
            else
                ++m_cur;
        }
        inline void _retreat(){
            --m_cur;
            while(String::IsValidUTF8ContinuationByte(*m_cur) && m_cur >= m_begin)
                --m_cur;
        }

    };

    inline UTF8Iterator beginUTF8(){ return UTF8Iterator(Data(), Data() + Length(), Data()); }
    inline UTF8Iterator endUTF8(){ return UTF8Iterator(Data(), Data() + Length(), Data() + Length()); }
    inline UTF8Iterator rbeginUTF8(){ return UTF8Iterator(Data(), Data() + Length(), Data() + Length() - 1); }
    inline UTF8Iterator rendUTF8(){ return UTF8Iterator(Data(), Data() + Length(), Data() - 1); }

    inline UTF8ConstIterator beginUTF8() const{ return UTF8ConstIterator(ConstData(), ConstData() + Length(), ConstData()); }
    inline UTF8ConstIterator endUTF8() const{ return UTF8ConstIterator(ConstData(), ConstData() + Length(), ConstData() + Length()); }
    inline UTF8ConstIterator rbeginUTF8() const{ return UTF8ConstIterator(ConstData(), ConstData() + Length(), ConstData() + Length() - 1); }
    inline UTF8ConstIterator rendUTF8() const{ return UTF8ConstIterator(ConstData(), ConstData() + Length(), ConstData() - 1); }


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
