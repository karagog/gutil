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
GUTIL_USING_CORE_NAMESPACE(DataObjects);


/** A null byte to mark the end of a string. */
#define STRING_TERMINATOR   static_cast<char>(0x00)

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





bool String::operator == (const String &s) const
{
    if(s.Length() != Length())
        return false;

    const char *cur1(s.ConstData()), *cur2(ConstData());
    for(GUINT32 i(0); i < Length(); ++i)
        if(*(cur1++) != *(cur2++))
            return false;
    return true;
}

bool String::operator == (const char *s) const
{
    GUINT32 sl( Length() );
    if(strlen(s) != sl)
        return false;

    const char *cur(ConstData());
    for(GUINT32 i(0); i < sl; ++i)
        if(*(s++) != *(cur++))
            return false;
    return true;
}

bool String::operator <  (const String &) const
{

}

bool String::operator <= (const String &) const
{

}

bool String::operator >  (const String &) const
{

}

bool String::operator >= (const String &) const
{

}

String String::operator + (const String &) const
{

}

String &String::operator += (const String &) const
{

}
