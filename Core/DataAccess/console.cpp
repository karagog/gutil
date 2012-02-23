/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "console.h"
#include <cstdio>
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL1(DataAccess);

void Console::Write(const char *d, OutputStreamEnum e)
{
    FILE *output_stream;
    switch(e)
    {
    case StandardOut:
        output_stream = stdout;
        break;
    case StandardError:
        output_stream = stderr;
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
    }

    fputs(d, output_stream);
}

void Console::WriteLine(const char *d, OutputStreamEnum e)
{
    if(d)
    {
        String s(d);
        GUINT32 indx = s.LastIndexOf('\n');
        if(indx != s.Length() - 1)
            s.Append('\n');
        Write(s, e);
    }
    else
    {
        Write("\n", e);
    }
}


/** We allocate a static buffer, so that calls to ReadLine
    won't over-allocate string sizes.
*/
static char __console_read_buffer[256];

GUINT32 Console::Read(GBYTE *buffer, GUINT32 buffer_len, GUINT32 bytes)
{
    GUINT32 ret(0);
    if(buffer_len > 0)
    {
        GUINT32 bytes_read(0);

        while(buffer_len-- != 1 && (bytes == UINT_MAX || bytes_read != bytes))
        {
            *buffer = getchar();
            ++bytes_read;

            if(bytes == UINT_MAX && String::IsWhitespace(*buffer))
                break;

            ++buffer;
            ++ret;
        }

        // Always terminate the buffer with a null char
        *buffer = '\0';

        if(bytes != UINT_MAX)
        {
            // If we didn't read all the bytes because of a small buffer, then
            //  throw away the rest of the bytes.
            while(bytes_read++ != bytes)
                getchar();
        }
    }
    return ret;
}

GINT32 Console::ReadInteger()
{
    GINT32 ret;
    if(0 == scanf("%d", &ret))
        THROW_NEW_GUTIL_EXCEPTION(ConversionException);
    return ret;
}

GFLOAT32 Console::ReadFloat()
{
    GFLOAT32 ret;
    if(0 == scanf("%f", &ret))
        THROW_NEW_GUTIL_EXCEPTION(ConversionException);
    return ret;
}

String Console::_read_until_terminator(bool (*is_terminator_char)(char))
{
    String ret;
    GUINT32 cnt(sizeof(__console_read_buffer));
    char *ptr( __console_read_buffer );
    G_FOREVER
    {
        char nextchar( getchar() );
        if(is_terminator_char(nextchar))
        {
            *ptr = '\0';
            ret.Append(__console_read_buffer, ptr - __console_read_buffer);
            break;
        }

        *ptr = nextchar;
        if(cnt == 0)
        {
            // Move the buffer into the return string so we can keep reading
            ret.Append(__console_read_buffer, sizeof(__console_read_buffer));
            cnt = sizeof(__console_read_buffer);
            ptr = __console_read_buffer;
        }
        else
        {
            --cnt, ++ptr;
        }
    }
    return ret;
}

bool Console::_is_end_line(char c)
{
    return c == '\n';
}

bool Console::_is_whitespace(char c)
{
    return String::IsWhitespace(c);
}


END_NAMESPACE_GUTIL1;
