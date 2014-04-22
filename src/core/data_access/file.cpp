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

#include "file.h"
#include <stdio.h>

NAMESPACE_GUTIL;


// casts our void pointer as the standard file handle type
#define H reinterpret_cast<FILE *>(h)


File::~File()
{
    if(IsOpen()) Close();
}

void File::Open(OpenModeEnum e)
{
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
    default:
        THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
    }

    h = fopen(m_filename, options);
    if(!h)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, String::Format("Unable to open file: '%s'", m_filename.ConstData()));
}

void File::Close()
{
    if(EOF == fclose(H))
    {
        // Do not throw an exception here, because some operating
        //  systems may automatically close a file before your program
        //  gets around to it.  In that case your program could
        //  end always with a crash.
        //THROW_NEW_GUTIL_EXCEPTION2(Exception, "Unable to close file");
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

void File::Delete(const char *filename)
{
    if(0 != remove(filename))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Unable to delete file");
}

GUINT32 File::Length() const
{
    GUINT32 ret(0);
    if(H)
    {
        long pos = ftell(H);
        fseek(H, 0, SEEK_END);
        ret = ftell(H);
        fseek(H, pos, SEEK_SET);
    }
    return ret;
}

void File::Seek(GUINT32 pos)
{
    if(0 != fseek(H, pos, SEEK_SET))
        THROW_NEW_GUTIL_EXCEPTION(GUtil::Exception);
}

GUINT32 File::Pos() const
{
    return H ? ftell(H) : 0;
}

GUINT32 File::Write(const GBYTE *data, GUINT32 len)
{
    GUINT32 ret = 0;
    if(1 == fwrite(data, len, 1, H)){
        if(!GetBufferedWrites())
            Flush();
        ret = len;
    }
    return ret;
}

void File::Flush()
{
    fflush(H);
}

GUINT32 File::Read(GBYTE *buffer, GUINT32 buffer_len, GUINT32 bytes_to_read)
{
    GUINT32 actually_read( Min(buffer_len, bytes_to_read) );
    if(1 != fread(buffer, actually_read, 1, H))
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "All data not read from file");
    if(actually_read < bytes_to_read)
        Seek(Pos() + (bytes_to_read - actually_read));
    return actually_read;
}

String File::Read(GUINT32 bytes)
{
    String ret;
    GUINT32 remaining( Length() - Pos() );

    ret.Resize(bytes == UINT_MAX ? remaining : bytes);
    Read(reinterpret_cast<GBYTE *>(ret.Data()), ret.Length(), remaining);
    return ret;
}


END_NAMESPACE_GUTIL;
