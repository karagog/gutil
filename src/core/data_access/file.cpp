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

#include "gutil_file.h"
#include <stdio.h>
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL1(DataAccess);


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

    m_handle = fopen(m_filename, options);
    if(!m_handle)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Unable to open file");
}

void File::Close()
{
    if(EOF == fclose(m_handle))
    {
        // Even if fclose fails, the handle is not associated with the file anymore
        m_handle = NULL;

        // Do not throw an exception here, because some operating
        //  systems may automatically close a file before your program
        //  gets around to it.  In that case your program could
        //  end always with a crash.
        //THROW_NEW_GUTIL_EXCEPTION2(Exception, "Unable to close file");
    }
    m_handle = NULL;
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
    if(m_handle)
    {
        long pos = ftell(m_handle);
        fseek(m_handle, 0, SEEK_END);
        ret = ftell(m_handle);
        fseek(m_handle, pos, SEEK_SET);
    }
    return ret;
}

void File::Seek(GUINT32 pos)
{
    if(0 != fseek(m_handle, pos, SEEK_SET))
        THROW_NEW_GUTIL_EXCEPTION(GUtil::Exception);
}

GUINT32 File::Pos() const
{
    return m_handle ? ftell(m_handle) : 0;
}

GUINT32 File::Write(const GBYTE *data, GUINT32 len)
{
    GUINT32 ret = 0;
    if(1 == fwrite(data, len, 1, m_handle)){
        if(!GetBufferedWrites())
            fflush(m_handle);
        ret = len;
    }
    return ret;
}

GUINT32 File::Read(GBYTE *buffer, GUINT32 buffer_len, GUINT32 bytes_to_read)
{
    GUINT32 actually_read( Min(buffer_len, bytes_to_read) );
    if(1 != fread(buffer, actually_read, 1, m_handle))
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


END_NAMESPACE_GUTIL1;
