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

#include "gstring.h"
#include "gassert.h"
#include <stdio.h>
#include <cstring>
GUTIL_USING_CORE_NAMESPACE(DataObjects);


/** A null byte to mark the end of a string. */
#define STRING_TERMINATOR   static_cast<char>(0x00)


String::String(const char *d, int len)
    :Vector<char>(len == -1 ? (len = strlen(d)) + 1 : len + 1)
{
    memcpy(Data(), d, len);
    Data()[len] = '\0';
    set_length(len);
}

String::String(char c, int len)
    :Vector<char>(len + 1)
{
    char *cur( Data() );
    while(len-- > 0) *(cur++) = c;
    Data()[len] = '\0';
    set_length(len);
}

String::String(const Vector<char>::const_iterator &b, const Vector<char>::const_iterator &e)
{
    GINT32 len(e - b);
    if(len != INT_MAX)
    {
        Reserve(gAbs(len) + 1);

        GINT8 inc(len >= 0 ? 1 : -1);
        char *mine(len >= 0 ? Data() : Data() + (gAbs(len) - 1));
        const char *cur( b.Current() );
        const char *end( e.Current() );

        while(cur != end)
        {
            *mine = *cur;
            cur += inc;
            mine += inc;
        }

        set_length(gAbs(len));
        operator[](gAbs(len)) = '\0';
    }
}

String::String(const UTF8ConstIterator &b, const UTF8ConstIterator &e)
{
    GUINT32 byte_len(UTF8ConstIterator::ByteDistance(b, e));
    if(byte_len != UINT_MAX)
    {
        Reserve(byte_len + 1);
        set_length(byte_len);

        UTF8Iterator my_cur( beginUTF8() );
        UTF8ConstIterator cur(b);
        while(cur != e)
        {
            my_cur.Copy(cur.Current(), e.Current());
            ++my_cur;
            ++cur;
        }

        operator[](byte_len) = '\0';
    }
}

String::String(const Vector<char> &s)
    :Vector<char>(s.Length() + 1)
{
    memcpy(Data(), s.ConstData(), s.Length());
    Data()[s.Length()] = '\0';
    set_length(s.Length());
}

String::String(const String &s)
    :Vector<char>(s.Length() + 1)
{
    memcpy(Data(), s.ConstData(), s.Length());
    Data()[s.Length()] = '\0';
    set_length(s.Length());
}

String &String::Insert(const String &s, GUINT32 indx)
{
    if(indx > Length())
        THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

    GUINT32 len( Length() );
    GUINT32 sl( s.Length() );

    // Reserve room for the extra null-terminating byte
    if(len + sl + 1 > Capacity())
        Reserve(len + sl + 1);

    Vector<char>::Insert(s, indx);

    // Always set the null-terminating byte.
    //  NOTE: This is 1 beyond the bounds of the vector, but since we have
    //  reserved space for it it's okay to set this value.
    Vector<char>::operator [](len + sl) = '\0';
    return *this;
}

String &String::Insert(const char *c, GUINT32 sz, GUINT32 indx)
{
    if(indx > Length())
        THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

    GUINT32 len( Length() );

    // Reserve room for the extra null-terminating byte
    if(len + sz + 1 > Capacity())
        Reserve(len + sz + 1);

    Vector<char>::Insert(c, sz, indx);

    // Always set the null-terminating byte.
    //  NOTE: This is 1 beyond the bounds of the vector, but since we have
    //  reserved space for it it's okay to set this value.
    Vector<char>::operator [](len + sz) = '\0';
    return *this;
}

String String::ToLower() const
{
    String ret(*this);
    char *c(ret.Data());
    if(c)
    {
        for(GUINT32 i(0); i < Length(); ++i, ++c)
            *c = ToLower(*c);
    }
    return ret;
}

String String::ToUpper() const
{
    String ret(*this);
    char *c(ret.Data());
    if(c)
    {
        const GUINT32 len(Length());
        for(GUINT32 i(0); i < len; ++i, ++c)
            *c = ToUpper(*c);
    }
    return ret;
}

String String::vFormat(const char *fmt, va_list args)
{
    String ret;

    // Call vsnprintf with a 0 length string, so that it tells us how large the
    //  string would be.  This is a design decision NOT to use a heuristic to
    //  guess at the length.  There is no good way to know how big the resulting
    //  string will be, so for simplicity and consistency's sake I go with the
    //  easy implementation where snprintf just tells me how big it will be.
    int new_len( vsnprintf(NULL, 0, fmt, args) );

    if(new_len < 0)
    {
        // If there's an error in translation, then return the format string
        //  Note: With old versions of the C Standard Library snprintf may
        //   return -1 if the character buffer is not large enough.  I don't
        //   want to support old standards, so I call this an error case.
        ret = fmt;
    }
    else
    {
        // +1 to account for the terminating null character
        ret.Reserve(new_len + 1);
        ret.set_length(new_len);

        vsnprintf(ret.Data(), new_len + 1, fmt, args);
    }

    va_end(args);
    return ret;
}

String String::Replace(const char *find, const char *replace, bool case_sensitive) const
{
    String ret(*this);
    String this_copy(*this);
    GUINT32 len( strlen(find) );
    if(!IsEmpty() && len > 0)
    {
        GUINT32 rlen( strlen(replace) );
        String find_copy(find);

        if(!case_sensitive)
        {
            find_copy = find_copy.ToLower();
            this_copy = this_copy.ToLower();
        }

        // If the replacement string is smaller, then start from the end of the string
        if(rlen < len)
        {
            GUINT32 last_index( this_copy.LastIndexOf(find_copy) );
            while(last_index != UINT_MAX)
            {
                memmove(ret.Data() + last_index + rlen, ret.Data() + last_index + len,
                        this_copy.Length() - (last_index + len));
                memmove(this_copy.Data() + last_index + rlen, this_copy.Data() + last_index + len,
                        this_copy.Length() - (last_index + len));

                memcpy(ret.Data() + last_index, replace, rlen);

                // This step is necessary so the IndexOf function always searches
                //  within the correct bounds
                this_copy.set_length(this_copy.Length() - (len - rlen));

                if(last_index < len)
                    last_index = UINT_MAX;
                else
                    last_index = this_copy.LastIndexOf(find_copy, last_index - len);
            }
        }
        else // Start from the beginning of the string
        {
            GUINT32 last_index( this_copy.IndexOf(find_copy) );
            while(last_index != UINT_MAX)
            {
                if(this_copy.Length() + (rlen - len) + 1 > ret.Capacity())
                {
                    ret.Reserve(this_copy.Length() + rlen - len + 1);
                    this_copy.Reserve(this_copy.Length() + rlen - len + 1);
                }

                memmove(ret.Data() + last_index + rlen, ret.Data() + last_index + len,
                        this_copy.Length() - (last_index + len));
                memmove(this_copy.Data() + last_index + rlen, this_copy.Data() + last_index + len,
                        this_copy.Length() - (last_index + len));

                memcpy(ret.Data() + last_index, replace, rlen);

                // This step is necessary so the IndexOf function always searches
                //  within the correct bounds
                this_copy.set_length(this_copy.Length() + rlen - len);

                if(last_index > this_copy.Length() - len)
                    last_index = UINT_MAX;
                else
                    last_index = this_copy.IndexOf(find_copy, last_index + rlen);
            }
        }
    }

    ret.set_length(this_copy.Length());
    ret[this_copy.Length()] = '\0';
    return ret;
}

GUINT32 String::IndexOf(const char *s, GUINT32 start, GUINT32 slen) const
{
    GUINT32 ret(UINT_MAX);
    if(slen == UINT_MAX)
        slen = strlen(s);

    if(Length() > 0 && slen > 0 && Length() >= slen)
    {
        const char *c( ConstData() + start );
        for(; start < Length() - (slen - 1); ++start, ++c)
        {
            if(0 == _string_compare(s, c, slen))
            {
                ret = start;
                break;
            }
        }
    }
    return ret;
}

GUINT32 String::LastIndexOf(const char *s, GUINT32 start, GUINT32 slen) const
{
    GUINT32 ret(UINT_MAX);
    if(slen == UINT_MAX)
        slen = strlen(s);

    if(Length() > 0 && slen > 0 && Length() >= slen)
    {
        if(start == UINT_MAX)
            start = Length() - slen;

        if(start < Length())
        {
            const char *c;
            if(Length() - start < slen)
                c = ConstData() + Length() - (slen - 1);
            else
                c = ConstData() + start;

            start += 1; // This helps our loop condition, because it's an unsigned int
            for(; start > 0; --start, --c)
            {
                if(0 == _string_compare(s, c, slen))
                {
                    ret = start - 1;
                    break;
                }
            }
        }
    }
    return ret;
}


bool String::ToBool(bool *ok) const
{
    GINT32 ret(0);
    int res( sscanf(ConstData(), "%d", &ret) );
    if(ok)
        *ok = 1 == res;
    return ret != 0;
}

GINT32 String::ToInt(bool *ok) const
{
    GINT32 ret(0);
    int res( sscanf(ConstData(), "%d", &ret) );
    if(ok)
        *ok = 1 == res;
    return ret;
}

GFLOAT32 String::ToFloat(bool *ok) const
{
    GFLOAT32 ret(0);
    int res( sscanf(ConstData(), "%f", &ret) );
    if(ok)
        *ok = 1 == res;
    return ret;
}

String String::Trim() const
{
    GUINT32 i(Length());
    UTF8ConstIterator iter_front( beginUTF8() );
    UTF8ConstIterator iter_back( rbeginUTF8() );
    bool stop_front(false);
    bool stop_back(false);

    while(i-- != 0 && (!stop_front || !stop_back))
    {
        if(!stop_front)
        {
            if(IsWhitespace(iter_front.Current()))
                ++iter_front;
            else
                stop_front = true;
        }

        if(!stop_back)
        {
            if(IsWhitespace(iter_back.Current()))
                --iter_back;
            else
            {
                stop_back = true;
                iter_back++;
            }
        }
    }

    return String(iter_front, iter_back);
}


Vector<String> String::Split(const char *separator, bool keep_empty_parts) const
{
    Vector<String> ret;
    GUINT32 len( strlen(separator) );
    GUINT32 i( IndexOf(separator, 0, len) );
    GUINT32 prev( 0 );

    while(i != UINT_MAX)
    {
        GUINT32 diff( i - prev );
        if(keep_empty_parts || diff > 0)
            ret.PushBack(String(ConstData() + prev, diff));

        prev = i + len;
        i = IndexOf(separator, prev, len);
    }

    if(Length() > 0)
    {
        GUINT32 diff( Length() - prev );
        if(keep_empty_parts || diff > 0)
            ret.PushBack(String(ConstData() + prev, diff));
    }
    return ret;
}

String String::Join(const Vector<String> &v, const char *separator, GUINT32 len)
{
    String ret;
    if(v.Length() != 0)
    {
        if(len == UINT_MAX)
            len = strlen(separator);

        GUINT32 capacity((v.Length() - 1) * len + 1);
        for(Vector<String>::const_iterator iter = v.begin(); iter != v.end(); ++iter)
            capacity += iter->Length();

        ret.Reserve(capacity);

        for(GUINT32 i(0); i < v.Length(); ++i)
        {
            ret.Append(v[i]);
            if(i < v.Length() - 1)
                ret.Insert(separator, len, ret.Length());
        }
    }
    return ret;
}



String String::ToUTF8() const
{
    String ret(*this);
    char *cur( ret.Data() );
    GUINT32 i( ret.Length() );
    while(i-- != 0)
    {

        ++cur;
    }
    return ret;
}

bool String::IsValidUTF8(GINT32 *bad_bytes) const
{
    bool ret(true);
    UTF8ConstIterator iter( beginUTF8() );
    if(iter)
    {
        for(; iter != endUTF8() && (ret || bad_bytes); ++iter)
        {
            if(*iter.Current() == '\0' ||
                    !iter.IsValidUTF8Sequence())
            {
                ret = false;
                if(bad_bytes)
                    ++(*bad_bytes);
            }
        }
    }
    else if(bad_bytes)
    {
        *bad_bytes = 0;
    }
    return ret;
}

bool String::IsValidUTF8Sequence(const char *c, const char *e)
{
    bool ret(true);
    if(IsValidUTF8StartByte(*c))
    {
        while(++c < e)
        {
            if(!IsValidUTF8ContinuationByte(*c))
            {
                ret = false;
                break;
            }
        }
    }
    else
        ret = false;
    return ret;
}


GUINT32 String::LengthUTF8() const
{
    UTF8ConstIterator iter( beginUTF8() );
    GUINT32 cnt(0);
    while(iter)
    {
        ++cnt;
        ++iter;
    }
    return cnt;
}

String String::Chop(GUINT32 n) const
{
    if(n >= Length())
        return String();

    UTF8ConstIterator iter( endUTF8() );
    iter -= n;
    return String(beginUTF8(), iter);
}



bool String::operator == (const char *s) const
{
    if(strlen(s) != Length())
        return false;

    return 0 == _string_compare(*this, s, Length());
}

bool String::operator <  (const String &s) const
{
    int ret( _string_compare(*this, s, gMin(s.Length(), Length())) );
    if(ret == 0)
        return Length() < s.Length();
    return ret < 0;
}

bool String::operator <= (const String &s) const
{
    int ret( _string_compare(*this, s, gMin(s.Length(), Length())) );
    if(ret == 0)
        return Length() <= s.Length();
    return ret < 0;
}

bool String::operator >  (const String &s) const
{
    int ret( _string_compare(*this, s, gMin(s.Length(), Length())) );
    if(ret == 0)
        return Length() > s.Length();
    return ret > 0;
}

bool String::operator >= (const String &s) const
{
    int ret( _string_compare(*this, s, gMin(s.Length(), Length())) );
    if(ret == 0)
        return Length() >= s.Length();
    return ret > 0;
}


String operator + (const char *c, const String &s)
{
    GUINT32 sz( strlen(c) );
    String ret(sz + s.Length());
    ret.Insert(c, sz, 0);
    return ret.Append(s);
}









GUINT32 String::UnicodeValue(const char *start, GINT8 mb_len)
{
    GUINT32 ret(0);
    if(mb_len == -1)
        mb_len = MultiByteLength(*start);

    if(mb_len < 0 || mb_len > 7 || !IsValidUTF8StartByte(*start))
        ret = UINT_MAX;
    else
    {
        if(String::IsValidAscii(*start))
            ret = *start;
        else
        {
            GINT8 byte_count(0);
            GINT8 mask( 0x3F );
            GINT8 c;
            for(GINT8 i(mb_len - 1); i > 0; --i, ++byte_count)
            {
                c = start[GINT16(i)];
                if(IsValidUTF8ContinuationByte(c))
                    ret |= GINT32(mask & c) << (byte_count * 6);
                else
                {
                    ret = UINT_MAX;
                    break;
                }
            }

            // The first character gets special treatment, because it is the only byte that
            //  has a variable number of possible bits present
            if(ret != UINT_MAX)
            {
                mask = (1 << (7 - mb_len)) - 1;
                ret |= GINT32(mask & *start) << ((byte_count) * 6);
            }
        }
    }

    // Do some final sanity checks on the Unicode value.  Is it an impossible value,
    //  because it could be represented by a shorter byte sequence, etc...?
    if(ret != UINT_MAX)
    {

    }

    return ret;
}



#define ASCII_CAPITAL_LETTER_OFFSET     0x41
#define ASCII_LOWCASE_LETTER_OFFSET     0x61
#define ASCII_NUMBER_OFFSET             0x30

char String::Base64a2i(char c)
{
    if(ASCII_CAPITAL_LETTER_OFFSET <= c &&
            c < ASCII_CAPITAL_LETTER_OFFSET + 26)
    {
        // Capital letter A-Z
        return c - ASCII_CAPITAL_LETTER_OFFSET;
    }
    else if(ASCII_LOWCASE_LETTER_OFFSET <= c &&
            c < ASCII_LOWCASE_LETTER_OFFSET + 26)
    {
        // Lower case letter a-z
        return c - ASCII_LOWCASE_LETTER_OFFSET + 26;
    }
    else if(ASCII_NUMBER_OFFSET <= c &&
            c < ASCII_NUMBER_OFFSET + 10)
    {
        // Number 0-9
        return c - ASCII_NUMBER_OFFSET + 52;
    }
    else if(c == '+')
    {
        return 62;
    }
    else if(c == '/')
    {
        return 63;
    }
    else if(c == '=')
    {
        // A special padding character gets a special return value
        return -1;
    }
    else
    {
        // Unrecognized base 64 character
        THROW_NEW_GUTIL_EXCEPTION(Exception);
    }
}

char String::Base64i2a(char c)
{
    if(0 <= c && c < 26)
    {
        return ASCII_CAPITAL_LETTER_OFFSET + c;
    }
    else if(26 <= c && c < 52)
    {
        return ASCII_LOWCASE_LETTER_OFFSET + c - 26;
    }
    else if(52 <= c && c < 62)
    {
        return ASCII_NUMBER_OFFSET + c - 52;
    }
    else if(c == 62)
    {
        return '+';
    }
    else if(c == 63)
    {
        return '/';
    }
    else if(c == -1)
    {
        return '=';
    }
    else
    {
        // Unrecognized base 64 character
        THROW_NEW_GUTIL_EXCEPTION(Exception);
    }
}

String String::ToBase64(const char *str, GUINT32 len)
{
    if(len == UINT_MAX)
        len = strlen(str);

    GUINT32 capacity( (len << 2) / 3 );

    if(capacity & 0b0011)
    {
        capacity &= 0xFFFFFFFC;
        capacity += 4;
    }

    String ret(capacity + 1);

    char mem;
    char mask1;
    char mask2;
    const char orig_mask1 = 0b11111100 ;
    const char orig_mask2 = 0b00000011 ;
    int shift;
    GUINT32 i;
    for(i = 0; i < len; ++i, ++str)
    {
        char six_bits;

        if(i % 3 == 0)
        {
            mask1 = orig_mask1;
            mask2 = orig_mask2;
            mem = 0x00;
            shift = 0;
        }
        else
        {
            mask1 = mask1 << 2;
            mask2 = (mask2 << 2) | (char)0x3;   // Fill new spaces with 1's
            shift += 2;
        }

        six_bits = ((unsigned char)((((unsigned char)(*str & mask1)) >> shift) | (mem << (8 - shift)))) >> 2;
        mem = *str & mask2;

        ret.Append( Base64i2a(six_bits) );

        // Take care of adding the 4th base64 character after every third ASCII character
        if(i % 3 == 2)
            ret.Append( Base64i2a(mem) );
    }

    // Pad the end
    int r = i % 3;
    if(r != 0)
    {
        ret.Append( Base64i2a(mem << (4 - shift)) );
        while(r++ < 3)
            ret.Append( '=');
    }

    return ret;
}

String String::FromBase64(const char *str, GUINT32 len)
{
    if(len == UINT_MAX)
        len = strlen(str);

    // The string length must be a multiple of 4
    if(len & 0b0011)
        THROW_NEW_GUTIL_EXCEPTION(Exception);

    String ret( (len * 3) >> 2 );

    // Now translate the string
    for(GUINT32 i = 0; i < len; i += 4)
    {
        char a( Base64a2i(*(str++)) );
        char b( Base64a2i(*(str++)) );
        char c( Base64a2i(*(str++)) );
        char d( Base64a2i(*(str++)) );

        ret.Append( (a << 2) | (b >> 4) );

        if(c >= 0)
        {
            ret.Append( (b << 4) | (c >> 2) );

            if(d >= 0)
                ret.Append( (c << 6) | d );
            else if(i < (len - 4))
                // String is in unrecognized format
                THROW_NEW_GUTIL_EXCEPTION( Exception );
        }
        else if(d >= 0)
            // String is in unrecognized format
            THROW_NEW_GUTIL_EXCEPTION( Exception );
    }

    return ret;
}


String String::FromBase16(const char *str, GUINT32 len)
{
    if(len == UINT_MAX)
        len = strlen(str);

    // Input string must be a multiple of 2
    if(len & 1)
        THROW_NEW_GUTIL_EXCEPTION( Exception );

    String ret(len >> 1);
    for(GUINT32 i = 0; i < len; i += 2)
    {
        char c1 = *(str++);
        char c2 = *(str++);

        ret.Append((CharToHex(c1) << 4) | (CharToHex(c2)));
    }
    return ret;
}

String String::ToBase16(const char *str, GUINT32 len)
{
    if(len == UINT_MAX)
        len = strlen(str);

    String ret(len << 1);
    const char *e(str + len);

    while(str != e)
    {
        ret.Append( HexToChar((*str & 0xf0) >> 4) );
        ret.Append( HexToChar(*str & 0x0f) );
        ++str;
    }
    return ret;
}

char String::CharToHex(char c)
{
    // Make it upper-case if it was lower
    if(c >= (char)0x61)
        c = c - 0x20;

    char lnib, rnib;
    char ret;

    lnib = (c & 0xf0) >> 4;
    rnib = c & 0x0f;

    if((lnib == 0x03) && (rnib < 0x0a))
    {	// If it's between 0 and 9
        ret = rnib;
    }
    else if((c >= (char)0x41) && (c <= (char)0x46))
    {	// if it's A-F
        ret = 0x09 + rnib;
    }
    else if((c >= (char)0x61) && (c <= (char)0x66))
    {	// if it's a-f
        ret = 0x09 + rnib;
    }
    else
        // Unrecognized hex character
        THROW_NEW_GUTIL_EXCEPTION( Exception );

    return ret;
}

char String::HexToChar(char c)
{
    char ret;

    // Make sure it's a valid hex digit
    if(0 != (c & 0xf0))
        THROW_NEW_GUTIL_EXCEPTION( Exception );

    // If it's less than 10, display a number
    else if(c < 0x0A)
        ret = 0x30 + c;

    // But if it's 10 or over, display a character A-F
    else
        ret = 0x41 + (c - 0x0A);

    return ret;
}



// CryptoPP-dependent section

#ifdef GUTIL_ENCRYPTION

#include "cryptopp-5.6.1/cryptlib.h"
#include "cryptopp-5.6.1/filters.h"
#include "cryptopp-5.6.1/gzip.h"
#include "cryptopp-5.6.1/randpool.h"
#include "cryptopp-5.6.1/osrng.h"
#include "cryptopp-5.6.1/default.h"
#include "cryptopp-5.6.1/pwdbased.h"

/** Used to adapt my string into CryptoPP Sink. */
class GStringSink : public CryptoPP::Bufferless<CryptoPP::Sink>
{
    String &sref;
public:
    inline GStringSink(String &s) :sref(s){}

    size_t Put2(const byte *inString, size_t length, int messageEnd, bool blocking){
        sref.Append(reinterpret_cast<const char *>(inString), length);
        return 0;
    }
};


String String::Compress(CompressionLevelEnum level) const
{
    String ret(Length() + 1);   // Preallocate space for the return string
    bool skip_compression = Length() > 10000000;

    if(!skip_compression)
    {
        if(level < MinimumCompression || level > MaximumCompression)
            level = DefaultCompression;

        try
        {
            CryptoPP::Gzip zipper(new GStringSink(ret), level);
            zipper.Put(reinterpret_cast<const byte *>(ConstData()), Length());
            zipper.MessageEnd();
        }
        catch(const CryptoPP::Exception &ex)
        {
            GDEBUG(ex.GetWhat());
            THROW_NEW_GUTIL_EXCEPTION(Exception);
        }
    }

    if(skip_compression ||  ret.Length() >= Length())
    {
        // Leave it uncompressed, because we didn't gain anything by compression
        ret = "0";
        ret.Append(*this);
    }
    else
    {
        ret.Prepend("1");
    }
    return ret;
}

String String::Decompress() const
{
    GUINT32 len(Length());
    String ret(len);

    if(len > 0)
    {
        bool is_compressed( false );
        const char *start(ConstData());

        if(*start == '1')
        {
            is_compressed = true;
            ++start;
            --len;
        }
        else if(*start == '0')
        {
            ++start;
            --len;
        }
        else
        {
            // Treat any string without our marking as compressed, then you'll get an exception
            //  if we couldn't decompress it
            is_compressed = true;
        }

        if(is_compressed)
        {
            try
            {
                CryptoPP::StringSource(reinterpret_cast<const byte *>(start), len, true,
                                       new CryptoPP::Gunzip(new GStringSink(ret)));
            }
            catch(const CryptoPP::Exception &ex)
            {
                GDEBUG(ex.GetWhat());
                THROW_NEW_GUTIL_EXCEPTION(Exception);
            }
        }
        else
        {
            ret.Append(start, len);
        }
    }

    return ret;
}

String String::Encrypt(const GBYTE *key, GUINT32 len, EncryptionTypeEnum e) const
{
    byte padded_key[CryptoPP::SHA256::DIGESTSIZE];
    String ret(Length());   // A heuristic to estimate the length of the return string
    if(len == UINT_MAX)
        len = strlen(reinterpret_cast<const char *>(key));

    try
    {
        switch(e)
        {
        case DefaultEncryption:
        {
            CryptoPP::SHA256().CalculateDigest(padded_key, reinterpret_cast<const byte *>(key), len);
            CryptoPP::DefaultEncryptorWithMAC enc(reinterpret_cast<const byte *>(padded_key),
                                                  CryptoPP::SHA256::DIGESTSIZE, new GStringSink(ret));
            enc.Put(reinterpret_cast<const byte *>(ConstData()), Length());
            enc.MessageEnd();
            break;
        }
        case TripleDES_Encryption:
        {
            // Initialize the padded key
            CryptoPP::SHA256().CalculateTruncatedDigest(padded_key, CryptoPP::DES_EDE3::KEYLENGTH, reinterpret_cast<const byte *>(key), len);

            // Create a block-sized initialization vector with pseudo-random data
            byte init_vec[CryptoPP::DES_EDE3::BLOCKSIZE];
            CryptoPP::AutoSeededX917RNG<CryptoPP::DES_EDE3>().GenerateBlock(init_vec, CryptoPP::DES_EDE3::BLOCKSIZE);
            ret.Append(reinterpret_cast<const char *>(init_vec), CryptoPP::DES_EDE3::BLOCKSIZE);

            CryptoPP::CFB_Mode<CryptoPP::DES_EDE3>::Encryption cfb_mode(padded_key, CryptoPP::DES_EDE3::KEYLENGTH, init_vec);
            CryptoPP::StreamTransformationFilter enc(cfb_mode, new GStringSink(ret));

            enc.Put(reinterpret_cast<const byte *>(ConstData()), Length());
            enc.MessageEnd();
        }
            break;
        case AES_Encryption:
        {
            // Initialize the padded key
            CryptoPP::SHA256().CalculateDigest(padded_key, reinterpret_cast<const byte *>(key), len);

            // Create a block-sized initialization vector with pseudo-random data
            byte init_vec[CryptoPP::AES::BLOCKSIZE];
            CryptoPP::AutoSeededX917RNG<CryptoPP::AES>().GenerateBlock(init_vec, CryptoPP::AES::BLOCKSIZE);
            ret.Append(reinterpret_cast<const char *>(init_vec), CryptoPP::AES::BLOCKSIZE);

            CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfb_mode(padded_key, CryptoPP::AES::MAX_KEYLENGTH, init_vec);
            CryptoPP::StreamTransformationFilter enc(cfb_mode, new GStringSink(ret));

            enc.Put(reinterpret_cast<const byte *>(ConstData()), Length());
            enc.MessageEnd();
        }
            break;
        default:
            THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
            break;
        }
    }
    catch(const CryptoPP::Exception &ex)
    {
        GDEBUG(ex.GetWhat());
        THROW_NEW_GUTIL_EXCEPTION(Exception);
    }

    return ret;
}

String String::Decrypt(const GBYTE *key, GUINT32 len, EncryptionTypeEnum e) const
{
    byte padded_key[CryptoPP::SHA256::DIGESTSIZE];
    String ret(Length());
    if(len == UINT_MAX)
        len = strlen(reinterpret_cast<const char *>(key));

    try
    {
        switch(e)
        {
        case DefaultEncryption:
        {
            CryptoPP::SHA256().CalculateDigest(padded_key, reinterpret_cast<const byte *>(key), len);
            CryptoPP::DefaultDecryptorWithMAC decryptor(reinterpret_cast<const byte *>(padded_key), CryptoPP::SHA256::DIGESTSIZE, new GStringSink(ret));

            decryptor.Put(reinterpret_cast<const byte *>(ConstData()), Length());
            decryptor.MessageEnd();
        }
            break;
        case TripleDES_Encryption:
        {
            CryptoPP::SHA256().CalculateTruncatedDigest(padded_key, CryptoPP::DES_EDE3::KEYLENGTH, reinterpret_cast<const byte *>(key), len);

            CryptoPP::CFB_Mode<CryptoPP::DES_EDE3>::Decryption cfb_mode(padded_key, CryptoPP::DES_EDE3::KEYLENGTH, reinterpret_cast<const byte *>(ConstData()));
            CryptoPP::StreamTransformationFilter decryptor(cfb_mode, new GStringSink(ret));

            decryptor.Put(reinterpret_cast<const byte *>(ConstData()) + CryptoPP::DES_EDE3::BLOCKSIZE, Length() - CryptoPP::DES_EDE3::BLOCKSIZE);
            decryptor.MessageEnd();
        }
            break;
        case AES_Encryption:
        {
            CryptoPP::SHA256().CalculateDigest(padded_key, reinterpret_cast<const byte *>(key), len);

            CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfb_mode(padded_key, CryptoPP::AES::MAX_KEYLENGTH, reinterpret_cast<const byte *>(ConstData()));
            CryptoPP::StreamTransformationFilter decryptor(cfb_mode, new GStringSink(ret));

            decryptor.Put(reinterpret_cast<const byte *>(ConstData()) + CryptoPP::AES::BLOCKSIZE, Length() - CryptoPP::AES::BLOCKSIZE);
            decryptor.MessageEnd();
        }
            break;
        default:
            THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
            break;
        }
    }
    catch(const CryptoPP::Exception &ex)
    {
        GDEBUG(ex.GetWhat());
        THROW_NEW_GUTIL_EXCEPTION(Exception);
    }

    return ret;
}

String String::RandomString(GUINT32 num_bytes, GUINT32 seed)
{
    bool autoseed( seed == UINT_MAX );
    String ret(num_bytes);
    ret.set_length(num_bytes);

    CryptoPP::AutoSeededX917RNG<CryptoPP::AES> rng(false, autoseed);
    if(!autoseed)
        rng.IncorporateEntropy(reinterpret_cast<byte *>(&seed), sizeof(GUINT32));

    try
    {
        rng.GenerateBlock(reinterpret_cast<byte *>(ret.Data()), num_bytes);
    }
    catch(const CryptoPP::Exception &ex)
    {
        GDEBUG(ex.GetWhat());
        THROW_NEW_GUTIL_EXCEPTION(Exception);
    }

    return ret;
}


#endif // GUTIL_ENCRYPTION
