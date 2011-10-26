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
#include <cstdarg>
#include <stdio.h>
#include <cstring>
GUTIL_USING_CORE_NAMESPACE(DataObjects);


/** A null byte to mark the end of a string. */
#define STRING_TERMINATOR   static_cast<char>(0x00)


String::String(const char *d, int len)
    :Vector<char>(len == -1 ? (len = strlen(d)) + 1 : len + 1)
{
    memcpy(Data(), d, len);
    *(Data() + len) = '\0';
    set_length(len);
}

String::String(char c, int len)
    :Vector<char>(len + 1)
{
    char *cur( Data() );
    while(len-- > 0) *(cur++) = c;
    *(Data() + len) = '\0';
    set_length(len);
}

String::String(const Vector<char> &s)
    :Vector<char>(s.Length() + 1)
{
    memcpy(Data(), s.ConstData(), s.Length());
    *(Data() + s.Length()) = '\0';
    set_length(s.Length());
}

String::String(const String &s)
    :Vector<char>(s.Length() + 1)
{
    memcpy(Data(), s.ConstData(), s.Length());
    *(Data() + s.Length()) = '\0';
    set_length(s.Length());
}

String &String::operator = (const String &s)
{
    if(s.Length() + 1 > Capacity())
        Reserve(s.Length() + 1);
    Vector<char>::operator = (s);
    *(Data() + s.Length()) = '\0';
    set_length(s.Length());
    return *this;
}


String &String::Insert(const String &s, GUINT32 indx)
{
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


#define LOWERCASE_OFFSET 0x61
#define UPPERCASE_OFFSET 0x41

String &String::ToLower()
{
    char *c(Data());
    if(c)
    {
        const GUINT32 len(Length());
        for(GUINT32 i(0); i < len; ++i, ++c)
        {
            const char cpy( *c );
            if(UPPERCASE_OFFSET <= cpy && cpy < (UPPERCASE_OFFSET + 26))
                *c = cpy - (UPPERCASE_OFFSET - LOWERCASE_OFFSET);
        }
    }
    return *this;
}

String &String::ToUpper()
{
    char *c(Data());
    if(c)
    {
        const GUINT32 len(Length());
        for(GUINT32 i(0); i < len; ++i, ++c)
        {
            const char cpy( *c );
            if(LOWERCASE_OFFSET <= cpy && cpy < (LOWERCASE_OFFSET + 26))
                *c = cpy - (LOWERCASE_OFFSET - UPPERCASE_OFFSET);
        }
    }
    return *this;
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
        //   return -1 if the character buffer is not large enough.  I don't
        //   want to support old standards, so I call this an error case.
        ret = fmt;
    }
    else
    {
        // +1 to account for the terminating null character
        ret.Reserve(new_len + 1);
        ret.set_length(new_len);

        int final_length( vsnprintf(ret.Data(), new_len + 1, fmt, args) );
        GASSERT(final_length == new_len);
    }

    va_end(args);
    return ret;
}





bool String::operator == (const String &s) const
{
    if(s.Length() != Length())
        return false;

    return 0 == _string_compare(*this, s, Length());
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
