/*Copyright 2010-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <gutil/string.h>
#include <gutil/smartpointer.h>
#include <gutil/globals.h>
#include "hash.h"
#include <stdio.h>
#include <cstring>
using namespace std;

/** A null byte to mark the end of a string. */
#define STRING_TERMINATOR   static_cast<char>(0x00)


NAMESPACE_GUTIL;


String::String(const char *d, GUINT32 len)
    :Vector<char>(d == NULL ? (len = 0) :
                              (len == GUINT32_MAX ?
                                   (len = strlen(d)) + 1 : len + 1))
{
    if(0 < len){
        memcpy(Data(), d, len);
        set_length(len);
    }
    if(NULL != d)
        Data()[len] = '\0';
}

String::String(char c, GUINT32 len)
    :Vector<char>(0 == len ? 0 : len + 1)
{
    if(0 < len){
        set_length(len);
        Data()[len] = '\0';
        char *cur( Data() );
        while(len-- > 0) *(cur++) = c;
    }
}

String::String(const Vector<char>::const_iterator &b, const Vector<char>::const_iterator &e)
{
    GUINT32 len(e - b);
    if(len != GUINT32_MAX)
    {
        Reserve(Abs(len) + 1);

        GINT8 inc(len >= 0 ? 1 : GUINT32_MAX);
        char *mine(len >= 0 ? Data() : Data() + (Abs(len) - 1));
        const char *cur( b.Current() );
        const char *end( e.Current() );

        while(cur != end)
        {
            *mine = *cur;
            cur += inc;
            mine += inc;
        }

        set_length(Abs(len));
        operator[](Abs(len)) = '\0';
    }
}

String::String(const UTF8ConstIterator &b, const UTF8ConstIterator &e)
{
    GUINT32 byte_len(ByteDistance(b, e));
    if(byte_len != GUINT32_MAX)
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
{
    _copy_init(s);
}

String::String(const String &s)
{
    _copy_init(s);
}

String &String::operator = (const String &o)
{
    this->~String();
    new(this) String(o);
    return *this;
}

String &String::operator = (const string &o)
{
    this->~String();
    new(this) String(o);
    return *this;
}

String &String::operator = (const char *o)
{
    this->~String();
    new(this) String(o);
    return *this;
}

void String::_copy_init(const Vector<char> &s)
{
    if(!s.IsNull()){
        Resize(s.Length());
        memcpy(Data(), s.ConstData(), s.Length());
    }
}

void String::Resize(GUINT32 sz)
{
    if(Capacity() <= sz)
        Reserve(sz + 1);
    set_length(sz);
    operator[](sz) = '\0';
}

void String::Resize(GUINT32 sz, char c)
{
    GUINT32 sz_before(Size());
    Resize(sz);

    if(sz_before < sz)
    {
        char *ptr = Data() + sz_before;
        memset(Data() + sz_before, c, DataEnd() - ptr);
    }
}

String &String::Insert(const String &s, GUINT32 indx)
{
    if(indx > Length())
        throw IndexOutOfRangeException<>();

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
        throw IndexOutOfRangeException<>();

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
    for(UTF8Iterator iter(ret.beginUTF8()); iter != ret.endUTF8(); ++iter)
        ToLower(iter.Current(), iter.Current());
    return ret;
}

bool String::IsUpper(int u)
{
    bool ret = false;

    // Roman character
    if(RomanUpperCase <= u && u < (RomanUpperCase + 26))
        ret = true;

    // Extended ASCII
    else if(0x00C0 <= u && u < 0x00DF)
    {
        if(u != 0x00D0 && u != 0X00D7)
            ret = true;
    }

    // Latin extended
    else if(0x0100 <= u && u < 0x0234)
    {
        bool even( 0 == (u & 1) );
        if((((u <= 0x0136) || (0x014A <= u && u <= 0x0176) || (0x01DE <= u && u <= 0x01EE) || (0x01F4 <= u && u <= 0x0232 && u != 0x0220)) && even) ||
                (((0x0139 <= u && u <= 0x0147) || (0x0179 <= u && u <= 0x017D) || (0x01CD <= u && u <= 0x01DB)) && !even) ||
            (0x0182 == u ||
             0x0184 == u ||
             0x0187 == u ||
             0x0189 == u ||
             0x018B == u ||
             0x018E == u ||
             0x0191 == u ||
             0x0193 == u ||
             0x0196 == u ||
             0x0198 == u ||
             0x019A == u ||
             0x019D == u ||
             0x01A0 == u ||
             0x01A2 == u ||
             0x01A4 == u ||
             0x01A7 == u ||
             0x01AA == u ||
             0x01AC == u ||
             0x01AF == u ||
             0x01B1 == u ||
             0x01B3 == u ||
             0x01B5 == u ||
             0x01B8 == u))
            ret = true;
        else if(0x01B7 == u)
            ret = true;
    }

    // Greek character
    else if(GreekUpperCase <= u && u < (GreekUpperCase + 25))
    {
        ret = true;
    }

    return ret;
}

void String::ToLower(char *dest, const char *c)
{
    GINT8 blen = MultiByteLength(*c);
    GUINT32 u = UnicodeValue(c, blen);

    // Roman character
    if(RomanUpperCase <= u && u < (RomanUpperCase + 26))
    {
        UTF8CharFromUnicode(dest, u - (RomanUpperCase - RomanLowerCase));
    }

    // Extended ASCII
    else if(0x00C0 <= u && u < 0x00DF)
    {
        if(u != 0x00D0 && u != 0X00D7)
            UTF8CharFromUnicode(dest, u + 0x0020);
    }

    // Latin extended
    else if(0x0100 <= u && u < 0x0234)
    {
        bool even( 0 == (u & 1) );
        if((((u <= 0x0136) || (0x014A <= u && u <= 0x0176) || (0x01DE <= u && u <= 0x01EE) || (0x01F4 <= u && u <= 0x0232 && u != 0x0220)) && even) ||
                (((0x0139 <= u && u <= 0x0147) || (0x0179 <= u && u <= 0x017D) || (0x01CD <= u && u <= 0x01DB)) && !even) ||
            (0x0182 == u ||
             0x0184 == u ||
             0x0187 == u ||
             0x0189 == u ||
             0x018B == u ||
             0x018E == u ||
             0x0191 == u ||
             0x0193 == u ||
             0x0196 == u ||
             0x0198 == u ||
             0x019A == u ||
             0x019D == u ||
             0x01A0 == u ||
             0x01A2 == u ||
             0x01A4 == u ||
             0x01A7 == u ||
             0x01AA == u ||
             0x01AC == u ||
             0x01AF == u ||
             0x01B1 == u ||
             0x01B3 == u ||
             0x01B5 == u ||
             0x01B8 == u))
            UTF8CharFromUnicode(dest, u + 1);
        else if(0x01B7 == u)
            UTF8CharFromUnicode(dest, u + 3);
    }

    // Greek character
    else if(GreekUpperCase <= u && u < (GreekUpperCase + 25))
    {
        UTF8CharFromUnicode(dest, u - (GreekUpperCase - GreekLowerCase));
    }
}

String String::ToUpper() const
{
    String ret(*this);
    for(UTF8Iterator iter(ret.beginUTF8()); iter != ret.endUTF8(); ++iter)
        ToUpper(iter.Current(), iter.Current());
    return ret;
}

bool String::IsLower(int u)
{
    bool ret = false;

    // Roman character
    if(RomanLowerCase <= u && u < (RomanLowerCase + 26))
        ret = true;

    // Extended ASCII
    else if(0x00E0 <= u && u < 0x00FF)
    {
        if(u != 0x00DF && u != 0x00F0)
            ret = true;
    }

    // Latin extended
    else if(0x0101 <= u && u < 0x0234)
    {
        bool even( 0 == (u & 1) );
        if((((u <= 0x0137) || (0x014B <= u && u <= 0x0177) || (0x01DF <= u && u <= 0x01EF) || (0x01F5 <= u && u <= 0x0233 && u != 0x0221)) && !even) ||
                (((0x013A <= u && u <= 0x0148) || (0x017A <= u && u <= 0x017E) || (0x01CE <= u && u <= 0x01DC)) && even) ||
            (0x0183 == u ||
             0x0185 == u ||
             0x0188 == u ||
             0x018A == u ||
             0x018C == u ||
             0x018F == u ||
             0x0192 == u ||
             0x0194 == u ||
             0x0197 == u ||
             0x0199 == u ||
             0x019B == u ||
             0x019E == u ||
             0x01A1 == u ||
             0x01A3 == u ||
             0x01A5 == u ||
             0x01A8 == u ||
             0x01AB == u ||
             0x01AD == u ||
             0x01B0 == u ||
             0x01B2 == u ||
             0x01B4 == u ||
             0x01B6 == u ||
             0x01B9 == u))
            ret = true;
        else if(0x01BA == u)
            ret = true;
    }

    // Greek character
    else if(GreekLowerCase <= u && u < (GreekLowerCase + 25))
    {
        // The old sigma character maps to a capital Sigma
        if(u == 0x3C2) u = 0x3C3;
        ret = true;
    }

    return ret;
}

void String::ToUpper(char *dest, const char *c)
{
    GINT8 blen = MultiByteLength(*c);
    GUINT32 u = UnicodeValue(c, blen);

    // Roman character
    if(RomanLowerCase <= u && u < (RomanLowerCase + 26))
    {
        UTF8CharFromUnicode(dest, u - (RomanLowerCase - RomanUpperCase));
    }

    // Extended ASCII
    else if(0x00E0 <= u && u < 0x00FF)
    {
        if(u != 0x00DF && u != 0x00F0)
            UTF8CharFromUnicode(dest, u - 0x0020);
    }

    // Latin extended
    else if(0x0101 <= u && u < 0x0234)
    {
        bool even( 0 == (u & 1) );
        if((((u <= 0x0137) || (0x014B <= u && u <= 0x0177) || (0x01DF <= u && u <= 0x01EF) || (0x01F5 <= u && u <= 0x0233 && u != 0x0221)) && !even) ||
                (((0x013A <= u && u <= 0x0148) || (0x017A <= u && u <= 0x017E) || (0x01CE <= u && u <= 0x01DC)) && even) ||
            (0x0183 == u ||
             0x0185 == u ||
             0x0188 == u ||
             0x018A == u ||
             0x018C == u ||
             0x018F == u ||
             0x0192 == u ||
             0x0194 == u ||
             0x0197 == u ||
             0x0199 == u ||
             0x019B == u ||
             0x019E == u ||
             0x01A1 == u ||
             0x01A3 == u ||
             0x01A5 == u ||
             0x01A8 == u ||
             0x01AB == u ||
             0x01AD == u ||
             0x01B0 == u ||
             0x01B2 == u ||
             0x01B4 == u ||
             0x01B6 == u ||
             0x01B9 == u))
            UTF8CharFromUnicode(dest, u - 1);
        else if(0x01BA == u)
            UTF8CharFromUnicode(dest, u - 3);
    }

    // Greek character
    else if(GreekLowerCase <= u && u < (GreekLowerCase + 25))
    {
        // The old sigma character maps to a capital Sigma
        if(u == 0x3C2) u = 0x3C3;
        UTF8CharFromUnicode(dest, u - (GreekLowerCase - GreekUpperCase));
    }
}

String String::Format(const char *fmt, ...)
{
    String ret;
    va_list args;
    va_start(args, fmt);

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
        //   return GUINT32_MAX if the character buffer is not large enough.  I don't
        //   want to support old standards, so I call this an error case.
        ret = fmt;
    }
    else
    {
        // +1 to account for the terminating null character
        ret.Reserve(new_len + 1);
        ret.set_length(new_len);

        // Reset the varargs 'cause we're calling vsnprintf a second time
        va_end(args); va_start(args, fmt);

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
            while(last_index != GUINT32_MAX)
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
                    last_index = GUINT32_MAX;
                else
                    last_index = this_copy.LastIndexOf(find_copy, last_index - len);
            }
        }
        else // Start from the beginning of the string
        {
            GUINT32 last_index( this_copy.IndexOf(find_copy) );
            while(last_index != GUINT32_MAX)
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
                    last_index = GUINT32_MAX;
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
    GUINT32 ret(GUINT32_MAX);
    if(slen == GUINT32_MAX)
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
    GUINT32 ret(GUINT32_MAX);
    if(slen == GUINT32_MAX)
        slen = strlen(s);

    if(Length() > 0 && slen > 0 && Length() >= slen)
    {
        if(start == GUINT32_MAX)
            start = Length() - slen;

        if(start < Length())
        {
            const char *c;
            if(Length() - start < slen)
                c = ConstData() + Length() - (slen - 1);
            else
                c = ConstData() + start;

            ++start;
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

GUINT32 String::IndexOfUTF8(const char *s, GUINT32 start, GUINT32 slen) const
{
    GUINT32 ret(GUINT32_MAX);
    if(slen == GUINT32_MAX)
        slen = LengthUTF8(s);

    GUINT32 mb_len( MultiByteLength(*s) );
    GUINT32 lutf8( LengthUTF8() );

    if(lutf8 > 0 && slen > 0 && lutf8 >= slen)
    {
        UTF8ConstIterator iter(beginUTF8() + start);
        for(; start < lutf8 - (slen - 1); ++start, ++iter)
        {
            if(0 == _string_compare(s, iter.Current(), mb_len))
            {
                ret = start;
                break;
            }
        }
    }
    return ret;
}

GUINT32 String::LastIndexOfUTF8(const char *s, GUINT32 start, GUINT32 slen) const
{
    GUINT32 ret(GUINT32_MAX);
    if(slen == GUINT32_MAX)
        slen = LengthUTF8(s);
    GUINT32 mb_len( MultiByteLength(*s) );
    GUINT32 lutf8(LengthUTF8());

    if(lutf8 > 0 && slen > 0 && lutf8 >= slen)
    {
        if(start == GUINT32_MAX)
            start = lutf8 - slen;

        if(start < lutf8)
        {
            UTF8ConstIterator iter;
            if(LengthUTF8() - start < slen)
                iter = endUTF8() - (slen - 1);
            else
                iter = beginUTF8() + start;

            ++start;
            for(; start > 0; --start, --iter)
            {
                if(0 == _string_compare(s, iter.Current(), mb_len))
                {
                    ret = start - 1;
                    break;
                }
            }

            iter.~UTF8ConstIterator();
        }
    }
    return ret;
}


bool String::ToBool(bool *ok) const
{
    GUINT32 ret(0);
    int res( sscanf(ConstData(), "%d", &ret) );
    if(ok)
        *ok = 1 == res;
    return ret != 0;
}

GINT32 String::ToInt(bool *ok) const
{
    GUINT32 ret(0);
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

String String::Trimmed() const
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

    if(Length() == 0)
        return *this;
    else
        return String(iter_front, iter_back);
}


StringList String::Split(const char *separator, bool keep_empty_parts) const
{
    StringList ret;
    GUINT32 len( strlen(separator) );
    GUINT32 i( IndexOf(separator, 0, len) );
    GUINT32 prev( 0 );

    while(i != GUINT32_MAX)
    {
        GUINT32 diff( i - prev );
        if(keep_empty_parts || diff > 0)
            ret.Append(String(ConstData() + prev, diff));

        prev = i + len;
        i = IndexOf(separator, prev, len);
    }

    if(Length() > 0)
    {
        GUINT32 diff( Length() - prev );
        if(keep_empty_parts || diff > 0)
            ret.Append(String(ConstData() + prev, diff));
    }
    return ret;
}

String String::Join(const StringList &sl, const char *separator, GUINT32 len)
{
    String ret;
    if(sl.Length() != 0)
    {
        if(len == GUINT32_MAX)
            len = strlen(separator);

        GUINT32 capacity((sl.Length() - 1) * len + 1);
        for(auto &s : sl)
            capacity += s.Length();
        ret.Reserve(capacity);

        for(GUINT32 i(0); i < sl.Length(); ++i)
        {
            ret.Append(sl[i]);
            if(i < sl.Length() - 1)
                ret.Append(separator, len);
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

bool String::IsValidUTF8(GUINT32 *bad_bytes) const
{
    bool ret = true;
    if(ConstData())
        ret = IsValidUTF8(ConstData(), ConstData() + Length(), bad_bytes);
    return ret;
}

bool String::IsValidUTF8(const char *start, const char *end, GUINT32 *bad_bytes)
{
    bool ret(true);
    if(bad_bytes) *bad_bytes = 0;

    for(UTF8ConstIterator iter(start, end, start);
        iter && (ret || bad_bytes);
        ++iter)
    {
        if(*iter.Current() == '\0' ||
                !iter.IsValidUTF8Sequence())
        {
            ret = false;
            if(bad_bytes) ++(*bad_bytes);
        }
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


GUINT32 String::LengthUTF8(const char *c)
{
    GUINT32 cnt(0);
    char tmpchar(*c);
    while(tmpchar != '\0')
    {
        ++cnt;
        if(IsValidUTF8StartByte(tmpchar))
            c += MultiByteLength(tmpchar);
        else
            ++c;
        tmpchar = *c;
    }
    return cnt;
}

String &String::ChopUTF8(GUINT32 n)
{
    if(n >= Length())
        Clear();
    else
    {
        UTF8ConstIterator iter( endUTF8() );
        iter -= n;
        set_length(iter.Current() - ConstData());
        operator [](Length()) = '\0';
    }
    return *this;
}

String &String::Chop(GUINT32 n)
{
    if(n >= Length())
        Empty();
    else
    {
        GUINT32 len(Length());
        set_length(len - n);
        operator[](len - n) = '\0';
    }
    return *this;
}

int String::Compare(const char *lhs, const char *rhs)
{
    return strcmp(lhs, rhs);
}


bool String::operator == (const char *s) const
{
    if(strlen(s) != Length())
        return false;

    return 0 == _string_compare(*this, s, Length());
}

bool String::operator <  (const String &s) const
{
    int ret( _string_compare(*this, s, Min(s.Length(), Length())) );
    if(ret == 0)
        return Length() < s.Length();
    return ret < 0;
}

bool String::operator <= (const String &s) const
{
    int ret( _string_compare(*this, s, Min(s.Length(), Length())) );
    if(ret == 0)
        return Length() <= s.Length();
    return ret < 0;
}

bool String::operator >  (const String &s) const
{
    int ret( _string_compare(*this, s, Min(s.Length(), Length())) );
    if(ret == 0)
        return Length() > s.Length();
    return ret > 0;
}

bool String::operator >= (const String &s) const
{
    int ret( _string_compare(*this, s, Min(s.Length(), Length())) );
    if(ret == 0)
        return Length() >= s.Length();
    return ret > 0;
}









GUINT32 String::UnicodeValue(const char *start, GINT8 mb_len)
{
    GUINT32 ret(0);
    if(mb_len == -1)
        mb_len = MultiByteLength(*start);

    if(mb_len < 0 || mb_len > 7 || !IsValidUTF8StartByte(*start))
        ret = GUINT32_MAX;
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
                    ret |= ((GUINT32)mask & c) << (byte_count * 6);
                else
                {
                    ret = GUINT32_MAX;
                    break;
                }
            }

            // The first character gets special treatment, because it is the only byte that
            //  has a variable number of possible bits present
            if(ret != GUINT32_MAX)
            {
                mask = (1 << (7 - mb_len)) - 1;
                ret |= ((GUINT32)mask & *start) << ((byte_count) * 6);
            }
        }
    }

    // Do some final sanity checks on the Unicode value.  Is it an impossible value,
    //  because it could be represented by a shorter byte sequence, etc...?
    if(ret != GUINT32_MAX)
    {

    }

    return ret;
}

/** Given a unicode point, this returns the number of bytes necessary to
    represent it
*/
static int __get_utf8_bytes_necessary(int uc_value)
{
    int ret(-1);
    int fsb = FSB32(uc_value);
    if(fsb == 0xFF || fsb < 7)
        ret = 1;
    else if(fsb < 11)
        ret = 2;
    else if(fsb < 16)
        ret = 3;
    else if(fsb < 21)
        ret = 4;
    else if(fsb < 26)
        ret = 5;
    else if(fsb < 31)
        ret = 6;
    return ret;
}

int String::UTF8CharFromUnicode(char *dest, GUINT32 uc_value)
{
    int ret = __get_utf8_bytes_necessary(uc_value);
    switch(ret)
    {
    case 1:
        // ASCII character, 1 byte
        *dest = uc_value;
        break;
    case 2:
        // 2-byte character
        dest[0] = 0xC0 | (uc_value >> 6);
        dest[1] = 0x80 | (0x3F & uc_value);
        break;
    case 3:
        // 3-byte character
        dest[0] = 0xE0 | (uc_value >> 12);
        dest[1] = 0x80 | (0x3F & (uc_value >> 6));
        dest[2] = 0x80 | (0x3F & uc_value);
        break;
    case 4:
        // 4-byte character
        dest[0] = 0xF0 | (uc_value >> 18);
        dest[1] = 0x80 | (0x3F & (uc_value >> 12));
        dest[2] = 0x80 | (0x3F & (uc_value >> 6));
        dest[3] = 0x80 | (0x3F & uc_value);
        break;
    case 5:
        // 5-byte character
        dest[0] = 0xF8 | (uc_value >> 24);
        dest[1] = 0x80 | (0x3F & (uc_value >> 18));
        dest[2] = 0x80 | (0x3F & (uc_value >> 12));
        dest[3] = 0x80 | (0x3F & (uc_value >> 6));
        dest[4] = 0x80 | (0x3F & uc_value);
        break;
    case 6:
        // 6-byte character
        dest[0] = 0xFC | (uc_value >> 30);
        dest[1] = 0x80 | (0x3F & (uc_value >> 24));
        dest[2] = 0x80 | (0x3F & (uc_value >> 18));
        dest[3] = 0x80 | (0x3F & (uc_value >> 12));
        dest[4] = 0x80 | (0x3F & (uc_value >> 6));
        dest[5] = 0x80 | (0x3F & uc_value);
        break;
    default:
        // The value is not in the valid Unicode space
        throw Exception<>();
        break;
    }
    return ret;
}

String &String::AppendUnicode(int uc)
{
    char tmp[6];
    return Append(tmp, UTF8CharFromUnicode(tmp, uc));
}



#define ASCII_NUMBER_OFFSET             0x30

char String::Base64a2i(char c)
{
    if(RomanUpperCase <= c &&
            c < RomanUpperCase + 26)
    {
        // Capital letter A-Z
        return c - RomanUpperCase;
    }
    else if(RomanLowerCase <= c &&
            c < RomanLowerCase + 26)
    {
        // Lower case letter a-z
        return c - RomanLowerCase + 26;
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
        throw Exception<>(String::Format("Unrecognized base-64 character: %c", c));
    }
}

char String::Base64i2a(char c)
{
    if(0 <= c && c < 26)
    {
        return RomanUpperCase + c;
    }
    else if(26 <= c && c < 52)
    {
        return RomanLowerCase + c - 26;
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
        throw Exception<>(String::Format("Unrecognized base-64 character: %x", c));
    }
}

String String::ToBase64(const char *str, GUINT32 len)
{
    if(len == GUINT32_MAX)
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
    if(len == GUINT32_MAX)
        len = strlen(str);

    if(len & 0b0011)
        throw Exception<>("Invalid Input: String length must be a multiple of 4");

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
                throw Exception<>();
        }
        else if(d >= 0)
            // String is in unrecognized format
            throw Exception<>();
    }

    return ret;
}


String String::FromBase16(const char *str, GUINT32 len)
{
    if(len == GUINT32_MAX)
        len = strlen(str);

    if(len & 1)
        throw Exception<>("Invalid Input: String length must be a multiple of 2");

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
    if(len == GUINT32_MAX)
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
        throw Exception<>(String::Format("Unrecognized HEX character: %c", c));

    return ret;
}

char String::HexToChar(char c)
{
    char ret;

    // Make sure it's a valid hex digit
    if(0 != (c & 0xf0))
        throw Exception<>(String::Format("Invalid HEX digit: %x", c));

    // If it's less than 10, display a number
    else if(c < 0x0A)
        ret = 0x30 + c;

    // But if it's 10 or over, display a character A-F
    else
        ret = 0x41 + (c - 0x0A);

    return ret;
}


String::const_iterator::const_iterator()
{}

String::const_iterator::const_iterator(const typename VectorImp<char>::const_iterator &o)
    :Vector<char>::const_iterator(o)
{}

String::const_iterator &String::const_iterator::operator = (const typename VectorImp<char>::const_iterator &o)
{
    this->~const_iterator();
    new(this) const_iterator(o);
    return *this;
}

String::iterator::iterator()
{}

String::iterator::iterator(const typename VectorImp<char>::iterator &o)
    :Vector<char>::iterator(o)
{}

String::iterator &String::iterator::operator = (const typename VectorImp<char>::iterator &o)
{
    this->~iterator();
    new(this) iterator(o);
    return *this;
}


END_NAMESPACE_GUTIL;


bool operator == (const char *c, const GUtil::String &s)
{
    return s == c;
}

bool operator != (const char *c, const GUtil::String &s)
{
    return s != c;
}


GUtil::String operator + (const char *c, const GUtil::String &s)
{
    GUINT32 sz( strlen(c) );
    GUtil::String ret(sz + s.Length());
    ret.Insert(c, sz, 0);
    return ret.Append(s);
}

namespace std
{
    GUINT32 hash<GUtil::String>::operator () (const GUtil::String &s) const noexcept{
        return GUtil::Hash::ComputeHash((byte const *)s.ConstData(), s.Length());
    }
}
