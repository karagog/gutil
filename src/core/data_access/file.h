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

#ifndef GUTIL_FILE_H
#define GUTIL_FILE_H

#include "gutil_iointerface.h"
#include "gutil_strings.h"
#include "gutil_exception.h"
#include <stdio.h>

NAMESPACE_GUTIL;


/** An encapsulation of a file, that gives easy access to file operations.

    This class is exception driven, so you should expect exceptions in
    failure cases and trap them.
*/
class File :
        public InputOutputInterface
{
    GUTIL_DISABLE_COPY(File);
public:

    /** Creates an empty file object.  You must use SetFilename before calling open. */
    File() :_p_BufferedWrites(true), m_handle(NULL){}
    /** Creates a file object with the given filename. */
    explicit File(const char *filename)
        :_p_BufferedWrites(true), m_filename(filename), m_handle(NULL){}
    virtual ~File();

    /** Closes the file if it was open and changes the filename*/
    void SetFilename(const char *filename){
        if(IsOpen()) Close();
        m_filename = filename;
    }

    /** Returns the current filename we're set to. */
    const String &Filename() const{ return m_filename; }

    /** Returns whether the file exists. */
    bool Exists() const{ return Exists(m_filename); }
    /** Returns whether the file exists. */
    static bool Exists(const char *filename);

    /** Closes the file and deletes it. */
    void Delete(){
        if(IsOpen()) Close();
        if(m_filename.Length() > 0) Delete(m_filename);
    }

    /** Deletes the file. */
    static void Delete(const char *filename);

    /** Describes different ways of opening a file. */
    enum OpenModeEnum
    {
        /** Open file with read access */
        OpenRead,

        /** Open file with write access.  Existing file data is erased. */
        OpenWrite,

        /** Open file with read/write access.  Existing file data is preserved. */
        OpenReadWrite,

        /** Open the file with read/write access.  Existing file data is erased.*/
        OpenReadWriteTruncate,

        /** Open file for writing, and start with the file pointer at the end. */
        OpenAppend,

        /** Open file for reading/writing, but no matter where the file pointer is,
            it will always be moved to the end of the file when you write.
        */
        OpenReadAndAppend
    };

    /** Opens the file with the given mode flags. */
    void Open(OpenModeEnum);

    /** Returns whether the file is open or not*/
    bool IsOpen() const{ return m_handle; }

    /** Closes the file if it is open. */
    void Close();

    /** The length of the file. */
    GUINT32 Length() const;
    /** The length of the file. */
    GUINT32 Size() const{ return Length(); }

    /** Write data to the file. */
    GUINT32 Write(const GBYTE *data, GUINT32 len);
    /** Write data to the file. */
    GUINT32 Write(const char *data, GUINT32 len = UINT_MAX){
        return Write(reinterpret_cast<GBYTE const *>(data),
                     len == UINT_MAX ? strlen(data) : len);
    }
    /** Write data to the file. */
    GUINT32 Write(const String &data){
        return Write(data.ConstData(), data.Length());
    }

    /** Read data from the file.  Return the number of bytes read. */
    GUINT32 Read(GBYTE *buffer, GUINT32 buf_len, GUINT32 bytes_to_read = UINT_MAX);
    /** Read data from the file and returns it in a string object. */
    String Read(GUINT32 bytes = UINT_MAX);

    /** Changes the position of the read pointer. */
    void Seek(GUINT32 pos);
    /** Returns the current position of the file pointer. */
    GUINT32 Pos() const;

    /** Satisfies the InputOutputInterface abstract interface. */
    virtual GUINT32 WriteBytes(const GBYTE *data, GUINT32 len){
        Write(data, len);
        return len;
    }

    /** Satisfies the InputOutputInterface abstract interface. */
    virtual GUINT32 ReadBytes(GBYTE *buffer, GUINT32 buffer_len, GUINT32 bytes_to_read){
        return Read(buffer, buffer_len, bytes_to_read);
    }


    /** Controls whether to buffer write outputs.  The default is true.
        If there is a write buffer, then data is not written to the file immediately,
        but when it's convenient.  If there is no buffer, then data is written
        immediately.
    */
    PROPERTY(BufferedWrites, bool);


private:

    String m_filename;
    FILE *m_handle;

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_FILE_H
