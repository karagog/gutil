/*Copyright 2010-2013 George Karagoulis

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

#include "gutil_vector.h"
#include "gutil_list.h"
#include <cstdarg>
#include <cstring>

NAMESPACE_GUTIL1(CryptoPP);
class EncryptionUtils;
END_NAMESPACE_GUTIL1;

#ifdef GUTIL_CORE_QT_ADAPTERS
#include <QString>
#endif // GUTIL_CORE_QT_ADAPTERS

NAMESPACE_GUTIL1(DataObjects);


class String;

/** A convenient typedef for a list of strings. */
typedef List<String> StringList;


/** Implements a contiguous string of characters, whose memory is managed automatically.

    The string is stored as a binary array of bytes, and the string functions are designed to operate
    on UTF-8 encoding (which includes normal ASCII).  It provides validation functions and a UTF-8 character
    iterator, that takes you through the string one multibyte character at a time.  Normal string functions
    like IndexOf() or Chop() are implemented to operate on multibyte UTF-8 characters, which is different
    than the std::string implementation.

    Also implements a suite of string helper functions, like for string compression and encryption.

    The requirements for this class are as follows:
    * Always maintains enough capacity for at least the amount of characters,
        plus the terminating null charater
    * Always maintains the terminating null, and at any given time throughout a
        string's life the character at index 'Length()' should be a '\0', given
        that the string has been initialized (not IsNull())
*/
class String :
        private Vector<char>
{
    friend class GUtil::CryptoPP::EncryptionUtils;
public:

    /** Creates a new empty string. */
    String() {}

    /** Creates an empty string with the given capacity (plus one for the null terminator). */
    explicit String(GINT32 capacity) :Vector<char>(capacity + 1) { Data()[capacity] = '\0'; }

    /** Creates a new string initialized with the data.
        \param d Points to the start of a char array
        \param len Specifies the length of the data.  If it is -1 then the string automatically
        finds the terminating null byte (it had better exist in this case!)
    */
    String(const char *d, GINT32 len = INT_MAX);

    /** Creates a new string initialized with the character repeated the specified number of times. */
    explicit String(char c, GINT32 len = 1);

    /** Constructs a string by copying the string between two Vector<char> iterators.
        \param b An iterator at the beginning of the string.
        \param e An iterator one past the end of the string.
        \note The end of the string must come after the beginning of the string
    */
    String(const Vector<char>::const_iterator &b, const Vector<char>::const_iterator &e);

    class UTF8ConstIterator;
    /** Constructs a string by copying the string between two UTF8 iterators.
        \param b An iterator at the beginning of the string.
        \param e An iterator one past the end of the string.
        \note The end of the string must come after the beginning of the string
    */
    String(const UTF8ConstIterator &b, const UTF8ConstIterator &e);

    /** Basically a copy constructor, but for the base type. */
    String(const Vector<char> &s);

    /** Copy constructor. */
    String(const String &s);

    /** Assignment operator. */
    String &operator = (const String &s){
        Empty();
        Append(s);
        return *this;
    }
    /** Assignment operator. */
    String &operator = (const char *c){
        Empty();
        Append(c);
        return *this;
    }
    /** Assignment operator. */
    String &operator = (char c){ char cpy[] = {c, '\0'}; return operator = (cpy);}

    /** The length of the string. */
    GINT32 Length() const{ return Vector<char>::Length(); }
    /** The length of the string. */
    GINT32 Size() const{ return Vector<char>::Length(); }

    /** Resizes the string, but does not initialize any new bytes. */
    void Resize(GINT32 sz);
    /** Resizes the string and fills any uninitialized bytes with the given char. */
    void Resize(GINT32 sz, char c){
        GINT32 sz_before(Size());
        Resize(sz);
        char *ptr(Data() + sz_before);
        while(ptr != DataEnd())
            *(ptr++) = c;
    }

    /** The number of bytes we're capable of holding. */
    GINT32 Capacity() const{ return Vector<char>::Capacity(); }

    /** Reserves space for the given size string (including a null terminator. */
    void Reserve(GINT32 n){ Vector<char>::Reserve(n + 1); }

    /** The number of UTF-8 characters (may differ from the actual byte length of the string).
        \note This returns the number of valid UTF-8 characters plus the invalid bytes, read
        until the terminating null.
    */
    GINT32 LengthUTF8() const{ return LengthUTF8(ConstData()); }

    /** The number of UTF-8 characters (may differ from the actual byte length of the string).
        \note This returns the number of valid UTF-8 characters plus the invalid bytes, read
        until the terminating null.
    */
    static GINT32 LengthUTF8(const char *);

    /** Returns if the string is null, i.e. has not been initialized. */
    bool IsNull() const{ return ConstData() == NULL; }

    /** Returns if it is an empty string, including when it has not been initialized. */
    bool IsEmpty() const{ return Length() == 0; }

    /** Clears the string and reclaims the memory. */
    void Clear(){ Vector<char>::Clear(); }

    /** Returns a pointer to the start of the string. */
    char *Data(){ return Vector<char>::Data(); }
    /** Returns a pointer to the const start of the string. */
    const char *ConstData() const{ return Vector<char>::ConstData(); }
    /** Returns a pointer to one char past end of the string. */
    const char *DataEnd() const{ return Vector<char>::ConstDataEnd(); }


    /** Appends the string to this one and returns this. */
    String &Append(const String &s){ Insert(s, s.Length(), Length()); return *this; }

    /** Appends the string to this one and returns this. */
    String &Append(const char *s, GINT32 len = INT_MAX){
        Insert(s, len == INT_MAX ? strlen(s) : len, Length()); return *this;
    }

    /** Appends the character the specified number of times. */
    String &Append(char c, GINT32 cnt = 1){
        if(cnt == 1)
            Insert(&c, 1, Length());
        else
            Insert(String(c, cnt), Length());
        return *this;
    }

    /** Appends the string to this string and returns this. */
    String &operator << (const char *s){ Append(s); return *this; }

    /** Appends the string to this string and returns this. */
    String &operator << (const String &s){ Append(s); return *this; }

    /** Prepends the string to this one and returns this. */
    String &Prepend(const String &s){ Insert(s, 0); return *this; }

    /** Prepends the character the specified number of times. */
    String &Prepend(char c, GINT32 cnt = 1){
        if(cnt == 1)
            Insert(&c, 1, 0);
        else
            Insert(String(c, cnt), 0);
        return *this;
    }


    /** Inserts the string at the given index. */
    String &Insert(const String &s, GINT32 indx);

    /** Inserts the raw data at the given index. */
    String &Insert(const char *c, GINT32 sz, GINT32 indx);


    /** Removes the number of bytes starting at byte index 'indx'. */
    String &RemoveBytesAt(GINT32 byte_indx, GINT32 len_in_bytes = 1){
        Vector<char>::RemoveAt(byte_indx, len_in_bytes);
        *(Data() + Length()) = '\0';
        return *this;
    }

    /** Removes the number of characters (UTF-8) starting at the given character index.
        \note This operates on UTF-8 (multibyte) characters.  If you need to remove bytes then
        use RemoveBytes().
        \note In the worst case this is O(N), because it must evaluate each start byte to determine
        how many bytes are in the multibyte character.
    */
    String &RemoveAt(GINT32 char_indx, GINT32 len_in_chars = 1);

    /** Removes all instances of the given byte. */
    String &RemoveAll(GBYTE b){ Vector<char>::RemoveAll(b); return *this; }

    /** Removes all instances of the given character from Unicode.
        \param unicode_value The Unicode code-point you want to remove from the string.
        \note This version of the function operates on UTF-8 characters only.  Use another
        version to remove individual bytes.
    */
    String &RemoveAll(GINT32 unicode_value);


    /** Returns the left N characters of the string. */
    String Left(GINT32 N) const { return SubString(0, N); }
    /** Returns the right N characters of the string. */
    String Right(GINT32 N) const { return SubString(Length() - N, N); }

    /** Returns the substring starting at the given index and going for the given length. */
    String SubString(GINT32 index, GINT32 length) const{
        return String(ConstData() + index, length);
    }

    /** Returns the substring starting at the given index and going for the given length in UTF-8 characters. */
    String SubStringUTF8(GINT32 index, GINT32 length_utf8) const{
        UTF8ConstIterator iter(beginUTF8() + index);
        UTF8ConstIterator iter_end(iter + length_utf8);
        return String(iter, iter_end);
    }


    /** A reference to the unicode code space, telling us the starting indexes of various
        languages.
    */
    enum UnicodeCharacterOffsets
    {
        RomanLowerCase = 0x0061,
        RomanUpperCase  = 0x0041,

        GreekLowerCase = 0x03B1,
        GreekUpperCase = 0x0391
    };

    /** Returns a copy of this string with all upper case letters changed to lower case */
    String ToLower() const;

    /** Returns a copy of this string with all lower case letters changed to upper case */
    String ToUpper() const;

    /** Writes the lower case version of the multibyte UTF-8 character to the destination.
        the destination must be the same size as the character.

        \note Not all character sets are supported.  In theory they all could be,
        but I don't have time to worry about languages I don't use.  If you want
        your language's character set to be supported, then send me a list of all
        the upper case letters and their lower case correspondents.
    */
    static void ToLower(char *dest, const char *c);

    /** Writes the upper case version of the multibyte UTF-8 character to the destination.
        the destination must be the same size as the character.

        \note Not all character sets are supported.  In theory they all could be,
        but I don't have time to worry about languages I don't use.  If you want
        your language's character set to be supported, then send me a list of all
        the upper case letters and their lower case correspondents.
    */
    static void ToUpper(char *dest, const char *c);

    /** Returns true if the unicode point is an upper case character. */
    static bool IsUpper(int unicode_value);

    /** Returns true if the unicode point is a lower case character. */
    static bool IsLower(int unicode_value);

    /** Returns the index of the first instance of the character.
        \returns INT_MAX if not found
    */
    GINT32 IndexOf(char c, GINT32 start = 0) const{ return Vector<char>::IndexOf(c, start); }
    /** Returns the index of the first instance of the string.
        \returns INT_MAX if not found
    */
    GINT32 IndexOf(const String &s, GINT32 start = 0) const{
        return IndexOf(s.ConstData(), start, s.Length());
    }
    /** Returns the index of the first instance of the String.
        \returns INT_MAX if not found
    */
    GINT32 IndexOf(const char *, GINT32 start = 0, GINT32 string_length = INT_MAX) const;

    /** Returns the index of the last instance of the character.
        \returns INT_MAX if not found
    */
    GINT32 LastIndexOf(char c, GINT32 start = INT_MAX) const{ return Vector<char>::LastIndexOf(c, start); }
    /** Returns the index of the last instance of the string.
        \returns INT_MAX if not found
    */
    GINT32 LastIndexOf(const String &s, GINT32 start = INT_MAX) const{
        return LastIndexOf(s.ConstData(), start, s.Length());
    }
    /** Returns the index of the last instance of the string.
        \returns INT_MAX if not found
    */
    GINT32 LastIndexOf(const char *, GINT32 start = INT_MAX, GINT32 string_length = INT_MAX) const;


    /** Returns the UTF-8 index of the string.
        \returns INT_MAX if not found
    */
    GINT32 IndexOfUTF8(const String &s, GINT32 start = 0) const{
        return IndexOfUTF8(s.ConstData(), start, s.LengthUTF8());
    }
    /** Returns the UTF-8 index of the string.
        \param s A pointer to the starting byte of a UTF-8 string.  If you already know
        how long this string is (in UTF-8 characters), then you are encouraged to
        pass it as the string_length parameter to increase efficiency
        \param start Indicates the UTF-8 character index of this string at which to
        start searching.  By default we start at the beginning of the string (index 0)
        \param string_length The UTF-8 length of the search string (not the byte length!)
        This is only for optimization purposes; if you don't know the length of the
        string it will be determined automatically for you
        \returns INT_MAX if not found
    */
    GINT32 IndexOfUTF8(const char *s, GINT32 start = 0, GINT32 string_length = INT_MAX) const;

    /** Returns the last UTF-8 index of the string.
        \returns INT_MAX if not found
    */
    GINT32 LastIndexOfUTF8(const String &s, GINT32 start = INT_MAX) const{
        return LastIndexOfUTF8(s.ConstData(), start, s.Length());
    }
    /** Returns the last UTF-8 index of the string.
        \returns INT_MAX if not found
    */
    GINT32 LastIndexOfUTF8(const char *, GINT32 start = INT_MAX, GINT32 string_length = INT_MAX) const;

    /** Format a string using printf-style strings.  It is a static function, so
        to use it would look like this:

        String s = String::Format("Hello %s, my name is %s", "Suzy", "George")

        \param fmt A format string, which will be passed directly to snprintf, so
        all the formatting rules for that function apply here (see man page for details)

        \return The formatted string.  If the formatting fails for some reason
        then the format string itself is returned.
    */
    static String Format(const char *fmt, ...);


    /** Returns a copy of this string, which has been parsed linearly to replace
        any instance of 'find' with 'replace'
        \param find A null-terminated string to search for in this string
        \param replace A null-terminated string with which to replace 'find'
        \param case_sensitive Whether or not to search with case sensitivity. Replace
        will always retain the same case it had.
    */
    String Replace(const char *find, const char *replace, bool case_sensitive = true) const;

    /** Converts the string to a boolean. */
    bool ToBool(bool *ok = 0) const;
    /** Converts the string to an integer. */
    GINT32 ToInt(bool *ok = 0) const;
    /** Converts the string to a float. */
    GFLOAT32 ToFloat(bool *ok = 0) const;

    /** Constructs a string from a boolean. */
    static String FromBool(bool b){ return b ? "1" : "0"; }
    /** Constructs a string from an integer. */
    static String FromInt(GINT32 i){ return String::Format("%d", i); }
    /** Constructs a string from a float. */
    static String FromFloat(GFLOAT32 d){ return String::Format("%#f", d); }


    /** Chops the last N characters off this string, and returns a reference to it */
    String &Chop(GINT32 N);

    /** Chops the last N UTF-8 characters (or invalid bytes) and returns a reference to it. */
    String &ChopUTF8(GINT32 N);

    /** Truncates the string to the first N bytes, and returns a reference to this. */
    String &Truncate(GINT32 N){ Chop(Length() - N); return *this; }

    /** Truncates the string to the first N UTF-8 characters (or invalid bytes), and returns a reference to this. */
    String &TruncateUTF8(GINT32 N){ ChopUTF8(LengthUTF8() - N); return *this; }


    /** Removes whitespace characters from the front and back of this string.
        \return A reference to this string after trimming.
    */
    String Trimmed() const;

    /** Returns true if the multibyte character is considered whitespace. */
    static bool IsWhitespace(const char *c){
        GINT32 uc( UnicodeValue(c) );
        return (uc <= 0x20) || uc == 0x7F;
    }
    /** Returns true if the byte is considered whitespace. */
    static bool IsWhitespace(char c){
        char tmp[] = { c, '\0' };
        return IsWhitespace(tmp);
    }

    /** Returns true if the charater is a numeral (0-9) */
    static bool IsNumber(char c){
        return (char)0x30 <= c && (char)0x3A > c;
    }

    /** Splits the string using the given character delimiter.
        \param separator A single character delimiter
        \param keep_empty_parts Indicates whether or not to retain the empty strings
        that may result from the split (such is the case when two delimiters appear
        side-by-side)  Then you would have an empty placeholder spot in the list.
    */
    StringList Split(char separator, bool keep_empty_parts = true) const{
        char c[] = {separator, '\0'};
        return Split(c, keep_empty_parts);
    }

    /** Splits the string using the given string delimiter.
        \param separator A null-terminated string delimiter
        \param keep_empty_parts Indicates whether or not to retain the empty strings
        that may result from the split (such is the case when two delimiters appear
        side-by-side)  Then you would have an empty placeholder spot in the list.
    */
    StringList Split(const char *separator, bool keep_empty_parts = true) const;

    /** Creates a new string by joining the strings in the vector, using the given separator between them. */
    static String Join(const Vector<String> &v, char separator){ return Join(v, &separator, 1); }
    /** Creates a new string by joining the strings in the vector, using the given separator between them. */
    static String Join(const Vector<String> &v, const String &separator){ return Join(v, separator.ConstData(), separator.Length()); }
    /** Creates a new string by joining the strings in the vector, using the given separator between them. */
    static String Join(const Vector<String> &, const char *separator, GINT32 len = INT_MAX);



    /** Returns true if the character is an ASCII character (non-extended). */
    static bool IsValidAscii(char c){ return c >= 0;  }

    /** Returns true if the character is a valid UTF-8 start byte. */
    static bool IsValidUTF8StartByte(GINT8 b){ return !IsValidUTF8ContinuationByte(b); }

    /** Returns true if the character is a valid UTF-8 continuation byte. */
    static bool IsValidUTF8ContinuationByte(GINT8 b){
        return GINT8(0x80) == (b & GINT8(0xC0));
    }

    /** Returns true if the character sequence represents a valid UTF-8 multibyte character.
        \param start A pointer to the start of the multibyte character.
        \param end A pointer to one past the end of the multibyte character.
        \note This is designed to work on a single multibyte character, not a sequence of multibyte characters.
    */
    static bool IsValidUTF8Sequence(const char *start, const char *end);

    /** Returns the theoretical byte length of the multibyte character
        starting with start_byte
    */
    static GINT8 MultiByteLength(char start_byte){
        GINT8 msb( FSB8(~(start_byte)) );
        return msb == 7 ? 1 : 7 - msb;
    }

    /** Returns the Unicode codepoint for the multibyte character. */
    static GINT32 UnicodeValue(const char *multibyte_start, GINT8 multibyte_length = -1);

    /** Generates a UTF-8 multibyte character from a unicode code point.
        \param dest must be large enough to hold the multibyte char.  If you're only dealing
        with ascii then the length will always be 1.
        \param uc_value The unicode codepoint value
        \returns The number of bytes required to represent the UTF-8 character.
    */
    static int UTF8CharFromUnicode(char *dest, GINT32 uc_value);
    
    /** Appends a unicode character to the string as UTF-8. */
    String &AppendUnicode(int);

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
    String ToBase64() const{ return ToBase64(ConstData(), Length()); }

    /** Returns the base64 version of this string. */
    static String ToBase64(const char *, GINT32 len = INT_MAX);

    /** Returns the decoded version of this base64 string.
        If the string is not actually a base64 string then the behavior is undefined.
        \note The resulting string will be 3/4 as large
    */
    String FromBase64() const{ return FromBase64(ConstData(), Length()); }

    /** Returns the decoded version of the base64 string. */
    static String FromBase64(const char *, GINT32 len = INT_MAX);


    /** Given a base 64 character, it returns the number it represents. */
    static char Base64a2i(char);

    /** Given a number, it returns the base 64 character for it. */
    static char Base64i2a(char);


    /** Returns the base16 version of this string.
        \note The resulting string will be 2 times as large.
    */
    String ToBase16() const{ return ToBase16(ConstData(), Length()); }

    /** Returns the base16 version of the given string. */
    static String ToBase16(const char *, GINT32 len = INT_MAX);

    /** Returns the decoded version of this base16 string.
        \note The resulting string will be half as large.
    */
    String FromBase16() const{ return FromBase16(ConstData(), Length()); }

    /** Returns the decoded version of the base16 string. */
    static String FromBase16(const char *, GINT32 len = INT_MAX);

    /** Given a hex digit as an ascii character, this returns the 4-bit hex value. */
    static char CharToHex(char);

    /** Given a 4-bit hex value, this returns the ascii character used in the
        base16 numbering system.
        \note Always returns upper case characters
    */
    static char HexToChar(char);


    class const_iterator :
            public Vector<char>::const_iterator
    {
    public:
        const_iterator();
        const_iterator(const typename VectorImp<char>::const_iterator &);
        const_iterator &operator = (const typename VectorImp<char>::const_iterator &);
    };

    class iterator :
            public Vector<char>::iterator
    {
    public:
        iterator();
        iterator(const typename VectorImp<char>::iterator &);
        iterator &operator = (const typename VectorImp<char>::iterator &);
    };

    const_iterator begin() const{ return Vector<char>::begin(); }
    iterator begin(){ return Vector<char>::begin(); }
    const_iterator end() const{ return Vector<char>::end(); }
    iterator end(){ return Vector<char>::end(); }
    const_iterator rbegin() const{ return Vector<char>::rbegin(); }
    iterator rbegin(){ return Vector<char>::rbegin(); }
    const_iterator rend() const{ return Vector<char>::rend(); }
    iterator rend(){ return Vector<char>::rend(); }


    /** An iterator that helps you with parsing the String object.

        Strings are stored internally as UTF-8 characters, so this gives you
        all the functions necessary for interacting with the string one UTF-8 character
        at a time.
    */
    class UTF8Iterator
    {
        friend class UTF8ConstIterator;
        friend class String;
    protected:
        char *m_begin;
        char *m_end;
        char *m_cur;
    public:

        UTF8Iterator(char *begin, char *end, char *cur)
            :m_begin(begin), m_end(end), m_cur(cur)
        {}

        /** Returns a pointer to the byte on which we're currently positioned. */
        char *Current() const{ return m_cur; }

        /** If the iterator is valid, it copies one multibyte character (or invalid byte) starting at c into its current location.
            \param c
            \param very_end The very end of this string, to ensure we don't go beyond it
            during a search of a poorly formed
        */
        void Copy(const char *c, const char *very_end)
        {
            if(*this)
            {
                GINT8 len(String::IsValidUTF8StartByte(*c) ? String::MultiByteLength(*c) : 1);
                char *s(m_cur);
                char *e(s + len);
                while(s != e && c < very_end)
                {
                    *s = *c;
                    ++s, ++c;
                }
            }
        }

        bool IsValidStartByte() const{ return String::IsValidUTF8StartByte(*m_cur); }
        bool IsValidContinuationByte() const{ return String::IsValidUTF8ContinuationByte(*m_cur); }

        /** Returns true if we're positioned on a valid UTF-8 character sequence,
            but doesn't validate the Unicode character value itself.
        */
        bool IsValidUTF8Sequence() const{ return String::IsValidUTF8Sequence(m_cur, m_cur + ByteLength()); }

        /** Returns true if the character is ascii (non-extended) */
        bool IsValidAscii() const{ return m_cur && 0 < *m_cur; }

        /** Returns the Unicode codepoint of the current character, or -1 if it is
            on an invalid character.
        */
        GINT32 UnicodeValue() const{ return String::UnicodeValue(m_cur, ByteLength()); }

        /** Returns the number of bytes represented by this character.
            \note If currently positioned on an invalid UTF-8 character,
            then the return of this function is undefined.
            \sa IsValidStartByte()
        */
        GINT8 ByteLength() const{ GINT8 msb( FSB8(~(*m_cur)) );
                                         return msb == 7 ? 1 : 7 - msb;
                                        }

        /** Returns the number of bytes between the first and second iterators.
            \returns INT_MAX if the iterators aren't from the same string, or if the end comes before the beginning
        */
        static GINT32 ByteDistance(const UTF8Iterator &b, const UTF8Iterator &e){
            GINT32 ret;
            if(b.m_begin != e.m_begin || e.m_cur < b.m_cur) ret = INT_MAX;
            else ret = e.m_cur - b.m_cur;
            return ret;
        }

        /** Returns the Unicode codepoint of the current character, or -1 if it is
            on an invalid character.
        */
        GINT32 operator *() const{ return UnicodeValue(); }

        /** Prefix increment, advances the iterator and returns it. */
        UTF8Iterator &operator ++(){ _advance(); return *this; }
        /** Postfix increment, advances the iterator but returns the iterator it was before. */
        UTF8Iterator operator ++(int){ UTF8Iterator ret(*this); _advance(); return ret; }
        /** Increments the iterator n times. */
        UTF8Iterator &operator += (GINT32 n){ while(n-- != 0) _advance(); return *this; }
        /** Returns a copy of the iterator incremented n times. */
        UTF8Iterator operator +(GINT32 n){ UTF8Iterator ret(*this); while(n-- != 0) ret._advance(); return ret; }

        /** Prefix decrement, retreats the iterator and returns it. */
        UTF8Iterator &operator --(){ _retreat(); return *this; }
        /** Postfix decrement, retreats the iterator but returns the iterator it was before. */
        UTF8Iterator operator --(int){ UTF8Iterator ret(*this); _retreat(); return ret; }
        /** Decrements the iterator n times. */
        UTF8Iterator &operator -= (GINT32 n){ while(n-- != 0) _retreat(); return *this; }
        /** Returns a copy of the iterator decremented n times. */
        UTF8Iterator operator -(GINT32 n){ UTF8Iterator ret(*this); while(n-- != 0) ret._retreat(); return ret; }

        bool operator == (const UTF8Iterator &o) const{ return m_begin == o.m_begin && m_cur == o.m_cur; }
        bool operator != (const UTF8Iterator &o) const{ return !operator == (o); }

        bool operator == (const UTF8ConstIterator &o) const{ return m_begin == o.m_begin && m_cur == o.m_cur; }
        bool operator != (const UTF8ConstIterator &o) const{ return !operator == (o); }

        /** Returns true if we are positioned on a valid index.
            \note This says nothing about the character itself being a valid UTF-8 character.
        */
        operator bool () const{ return m_begin <= m_cur && m_cur < m_end; }


    protected:

        UTF8Iterator() :m_begin(0), m_end(0), m_cur(0){}


    private:

        void _advance(){
            if(IsValidUTF8Sequence() && m_cur + String::MultiByteLength(*m_cur) <= m_end)
                m_cur += ByteLength();
            else if(m_cur < m_end)
                ++m_cur;
        }
        void _retreat(){
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
        friend class UTF8Iterator;
        friend class String;
    protected:
        const char *m_begin;
        const char *m_end;
        const char *m_cur;
    public:

        UTF8ConstIterator(const char *begin, const char *end, const char *cur)
            :m_begin(begin), m_end(end), m_cur(cur)
        {}
        UTF8ConstIterator(const UTF8Iterator &iter)
            :m_begin(iter.m_begin), m_end(iter.m_end), m_cur(iter.m_cur)
        {}

        /** Returns a pointer to the byte on which we're currently positioned. */
        const char *Current() const{ return m_cur; }

        bool IsValidStartByte() const{ return String::IsValidUTF8StartByte(*m_cur); }


        bool IsValidContinuationByte() const{ return String::IsValidUTF8ContinuationByte(*m_cur); }


        /** Returns true if we're positioned on a valid UTF-8 character sequence,
            but doesn't validate the Unicode character value itself.
        */
        bool IsValidUTF8Sequence() const{ return m_cur && String::IsValidUTF8Sequence(m_cur, m_cur + ByteLength()); }

        /** Returns true if the character is ascii (non-extended) */
        bool IsValidAscii() const{ return m_cur && 0 < *m_cur; }

        /** Returns the Unicode codepoint of the current character, or -1 if it is
            on an invalid character.
        */
        GINT32 UnicodeValue() const{ return String::UnicodeValue(m_cur, ByteLength()); }

        /** Returns the number of bytes represented by this character.
            \note If currently positioned on an invalid UTF-8 character,
            then the return of this function is undefined.
            \sa IsValidStartByte()
        */
        GINT8 ByteLength() const{ return String::MultiByteLength(*m_cur); }

        /** Returns the number of bytes between the first and second iterators.
            \returns INT_MAX if the iterators aren't from the same string, or if the end comes before the beginning
        */
        static GINT32 ByteDistance(const UTF8ConstIterator &b, const UTF8ConstIterator &e){
            GINT32 ret;
            if(b.m_begin != e.m_begin || e.m_cur < b.m_cur) ret = INT_MAX;
            else ret = e.m_cur - b.m_cur;
            return ret;
        }

        /** Returns the Unicode codepoint of the current character, or -1 if it is
            on an invalid character.
        */
        GINT32 operator *() const{ return UnicodeValue(); }

        /** Prefix increment, advances the iterator and returns it. */
        UTF8ConstIterator &operator ++(){ _advance(); return *this; }
        /** Postfix increment, advances the iterator but returns the iterator it was before. */
        UTF8ConstIterator operator ++(int){ UTF8ConstIterator ret(*this); _advance(); return ret; }
        /** Increments the iterator n times. */
        UTF8ConstIterator &operator += (GINT32 n){ while(n-- != 0) _advance(); return *this; }
        /** Returns a copy of the iterator incremented n times. */
        UTF8ConstIterator operator + (GINT32 n){ UTF8ConstIterator ret(*this); while(n-- != 0) ret._advance(); return ret; }

        /** Prefix decrement, retreats the iterator and returns it. */
        UTF8ConstIterator &operator --(){ _retreat(); return *this; }
        /** Postfix decrement, retreats the iterator but returns the iterator it was before. */
        UTF8ConstIterator operator --(int){ UTF8ConstIterator ret(*this); _retreat(); return ret; }
        /** Decrements the iterator n times. */
        UTF8ConstIterator &operator -= (GINT32 n){ while(n-- != 0) _retreat(); return *this; }
        /** Returns a copy of the iterator decremented n times. */
        UTF8ConstIterator operator - (GINT32 n){ UTF8ConstIterator ret(*this); while(n-- != 0) ret._retreat(); return ret; }

        bool operator == (const UTF8ConstIterator &o) const{ return m_begin == o.m_begin && m_cur == o.m_cur; }
        bool operator != (const UTF8ConstIterator &o) const{ return !operator == (o); }

        bool operator == (const UTF8Iterator &o) const{ return m_begin == o.m_begin && m_cur == o.m_cur; }
        bool operator != (const UTF8Iterator &o) const{ return !operator == (o); }

        /** Returns true if we are positioned on a valid index.
            \note This says nothing about the character itself being a valid UTF-8 character.
        */
        operator bool () const{ return m_begin <= m_cur && m_cur < m_end && *m_cur != '\0'; }


    protected:

        UTF8ConstIterator() :m_begin(0), m_end(0), m_cur(0){}


    private:

        void _advance(){
            if(IsValidUTF8Sequence() && m_cur + String::MultiByteLength(*m_cur) <= m_end)
                m_cur += ByteLength();
            else if(m_cur < m_end)
                ++m_cur;
        }
        void _retreat(){
            --m_cur;
            while(String::IsValidUTF8ContinuationByte(*m_cur) && m_cur >= m_begin)
                --m_cur;
        }

    };

    UTF8Iterator beginUTF8(){ return UTF8Iterator(Data(), Data() + Length(), Data()); }
    UTF8Iterator endUTF8(){ return UTF8Iterator(Data(), Data() + Length(), Data() + Length()); }
    UTF8Iterator rbeginUTF8(){ return UTF8Iterator(Data(), Data() + Length(), Data() + Length() - 1); }
    UTF8Iterator rendUTF8(){ return UTF8Iterator(Data(), Data() + Length(), Data() - 1); }

    UTF8ConstIterator beginUTF8() const{ return UTF8ConstIterator(ConstData(), ConstData() + Length(), ConstData()); }
    UTF8ConstIterator endUTF8() const{ return UTF8ConstIterator(ConstData(), ConstData() + Length(), ConstData() + Length()); }
    UTF8ConstIterator rbeginUTF8() const{ return UTF8ConstIterator(ConstData(), ConstData() + Length(), ConstData() + Length() - 1); }
    UTF8ConstIterator rendUTF8() const{ return UTF8ConstIterator(ConstData(), ConstData() + Length(), ConstData() - 1); }


    /** Comparison operator. */
    bool operator == (const String &s) const{ return Vector<char>::operator == (s); }
    /** Comparison operator. */
    bool operator == (const char *s) const;

    /** Comparison operator. */
    bool operator != (const String &s) const{ return !(*this == s); }
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

    String operator + (const String &s) const{ return String(*this).Append(s); }
    String operator + (const char *s) const{ return String(*this).Append(s); }
    String &operator += (const String &s){ Append(s); return *this; }

    char &operator[] (GINT32 i){ return Vector<char>::operator [](i); }
    const char &operator[] (GINT32 i) const{ return Vector<char>::operator [](i); }

    char &At(GINT32 i){ return Vector<char>::At(i); }
    const char &At(GINT32 i) const{ return Vector<char>::At(i); }

    /** Useful cast operator to char * */
    operator const char* () const{ return ConstData(); }
    /** Useful cast operator to char * */
    operator char* (){ return Data(); }


#ifdef GUTIL_CORE_QT_ADAPTERS

    QString ToQString() const{ return QString::fromUtf8(ConstData(), Length()); }
    static String FromQString(const QString &s){ QByteArray ba(s.toUtf8()); return String(ba.constData(), ba.size()); }

    String(const QString &qs){ operator = (FromQString(qs)); }
    operator QString () const{ return ToQString(); }

#endif // GUTIL_CORE_QT_ADAPTERS


private:

    static int _string_compare(const char *lhs, const char *rhs, GINT32 len){
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


END_NAMESPACE_GUTIL1;


/** A convenience operator allows you to compare with const char * as a lhs value. */
bool operator == (const char *c, const GUtil::DataObjects::String &s);

/** A convenience operator allows you to compare with const char * as a lhs value. */
bool operator != (const char *c, const GUtil::DataObjects::String &s);

/** A convenience operator that allows you to create strings with the + operator. */
GUtil::DataObjects::String operator + (const char *, const GUtil::DataObjects::String &);


#endif // GUTIL_STRING_H
