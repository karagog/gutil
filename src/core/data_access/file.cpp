/*Copyright 2010-2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "file.h"
#include "gutil_extendedexception.h"
#include <stdio.h>
#include <malloc.h>
using namespace std;

NAMESPACE_GUTIL;


// casts our void pointer as the standard file handle type
#define H reinterpret_cast<FILE *>(h)


File::File(const char *filename, OpenModeEnum md)
    :_p_BufferedWrites(true),
      m_filename(filename),
      h(NULL)
{
    if(m_filename.IsEmpty())
        throw Exception<>("Filename cannot be empty");

    if(md != DoNotOpen)
        Open(md);
}

File::~File()
{
    if(IsOpen())
        Close();
}

void File::Open(OpenModeEnum e)
{
    if(IsOpen())
        throw Exception<>("File already open");

    const char *options;
    switch(e)
    {
    case OpenRead:
        options = "rb";
        break;
    case OpenWrite:
        options = "wb";
        break;
    case OpenReadWrite:
        options = "r+b";
        break;
    case OpenReadWriteTruncate:
        options = "w+b";
        break;
    case OpenAppend:
        options = "ab";
        break;
    case OpenReadAndAppend:
        options = "a+b";
        break;
    case DoNotOpen:
        return;
    default:
        throw NotImplementedException<>();
    }

    FILE *tmp;
    if(!(tmp = fopen(m_filename, options)))
        throw Exception<true>("Cannot open file", {
                                  {"filename", m_filename},
                                  {"error", strerror(errno)}
                              });
    h = tmp;
}

void File::Close()
{
    if(EOF == fclose(H))
    {
        // Do not throw an exception here, because some operating
        //  systems may automatically close a file before your program
        //  gets around to it.  In that case your program could
        //  end always with a crash.
        //throw Exception<>("Unable to close file");
    }

    // Even if fclose fails, the handle is not associated with the file anymore
    h = NULL;
}

bool File::Exists(const char *filename)
{
    bool res(false);

    // Test for existence by trying to open read-only
    FILE *f( fopen(filename, "r") );
    if(f){
        res = true;
        fclose(f);
    }
    return res;
}

void File::Touch(const char *filename)
{
    FILE *f = fopen(filename, "w");
    if(f == NULL)
        throw Exception<>(String::Format("Unable to touch '%s': %s", filename, strerror(errno)));
    fclose(f);
}

void File::Delete(const char *filename)
{
    if(0 != remove(filename))
        throw Exception<>("Unable to delete file");
}

GUINT64 File::Length() const
{
    GUINT64 ret(0);
    if(IsOpen()){
        long int pos = ftell(H);
        if(0 == fseek(H, 0, SEEK_END)){
            GUINT64 tmp = ftell(H);

            // Only set the return var if everything was successful, and
            //  we could seek back to our original location
            if(0 == fseek(H, pos, SEEK_SET))
                ret = tmp;
        }
    }
    else{
        // If the file is not opened yet, open it, read the length, then close it
        FILE *handle;
        if((handle = fopen(m_filename, "r"))){
            if(0 == fseek(handle, 0, SEEK_END))
                ret = ftell(handle);
            fclose(handle);
        }
    }
    return ret;
}

void File::Seek(GUINT64 pos)
{
    if(0 != fseek(H, pos, SEEK_SET))
        throw Exception<>();
}

GUINT64 File::Pos() const
{
    return H ? ftell(H) : 0;
}

GUINT32 File::Write(const GBYTE *data, GUINT32 len)
{
    GUINT32 ret = 0;
    ret = fwrite(data, 1, len, H);
    if(len != ret && ferror(H))
        throw Exception<>(String::Format("Error writing to file: %s", strerror(errno)));
    if(!GetBufferedWrites())
        Flush();
    return ret;
}

void File::Flush()
{
    fflush(H);
}

GUINT32 File::Read(GBYTE *buffer, GUINT32 buffer_len, GUINT32 bytes_to_read)
{
    bytes_to_read = Min(buffer_len, bytes_to_read);
    size_t bytes_read = fread(buffer, 1, bytes_to_read, H);
    if(bytes_read != bytes_to_read && ferror(H))
        throw Exception<>(String::Format("Read Error: %s", strerror(errno)));
    return bytes_read;
}

#define READ_BUFFER_SIZE 1024

String File::Read(GUINT32 bytes)
{
    String ret;
    char buf[READ_BUFFER_SIZE];
    size_t total_read = 0;
    while(0 == feof(H) && total_read < bytes){
        const size_t to_read = Min(sizeof(buf), bytes - total_read);
        const size_t bytes_read = fread(buf, 1, to_read, H);
        if(bytes_read != to_read && ferror(H))
            throw Exception<>(String::Format("Read Error: %s", strerror(errno)));
        ret.Append(buf, bytes_read);
        total_read += bytes_read;
    }
    return ret;
}

String File::ReadUntil(function<bool(String &)> pred)
{
    String ret;
    while(0 == feof(H) && !pred(ret)){
        char tmp;
        if(1 != fread(&tmp, 1, 1, H)){
            if(ferror(H))
                throw Exception<>(String::Format("Read Error: %s", strerror(errno)));
            GASSERT(feof(H));
        }
        else
            ret.Append(tmp);
    }
    return ret;
}

String File::ReadLine(GUINT32 max_len)
{
    return ReadUntil([&](String &s){
        bool chop = s.Length() > 0 && s[s.Length() - 1] == '\n';
        bool ret = chop || (max_len != GUINT32_MAX && s.Length() >= (max_len + 1));
        if(chop) s.Chop(1);
        return ret;
    });
}


END_NAMESPACE_GUTIL;
