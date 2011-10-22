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


String::String()
{
}

String::String(const char d, int len)
{
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
    if(strlen(s) != Length())
        return false;

    const char *cur(ConstData());
    for(GUINT32 i(0); i < Length(); ++i)
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
