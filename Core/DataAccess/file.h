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

NAMESPACE_GUTIL1(DataAccess);


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
    inline File() :_p_BufferedWrites(true), m_handle(NULL){}
    /** Creates a file object with the given filename. */
    inline explicit File(const char *filename)
        :_p_BufferedWrites(true), m_filename(filename), m_handle(NULL){}
    virtual ~File();

    /** Closes the file if it was open and changes the filename*/
    inline void SetFilename(const char *filename){
        if(IsOpen()) Close();
        m_filename = filename;
    }

    /** Returns the current filename we're set to. */
    inline const DataObjects::String &Filename() const{ return m_filename; }

    /** Returns whether the file exists. */
    inline bool Exists() const{ return Exists(m_filename); }
    /** Returns whether the file exists. */
    static bool Exists(const char *filename);

    /** Closes the file and deletes it. */
    inline void Delete(){
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
    inline bool IsOpen() const{ return m_handle; }

    /** Closes the file if it is open. */
    void Close();

    /** The length of the file. */
    GUINT32 Length() const;
    /** The length of the file. */
    inline GUINT32 Size() const{ return Length(); }

    /** Write data to the file. */
    void Write(const GBYTE *data, GUINT32 len);
    /** Write data to the file. */
    inline void Write(const char *data, GUINT32 len = UINT_MAX){
        Write(reinterpret_cast<GBYTE const *>(data),
              len == UINT_MAX ? strlen(data) : len);
    }
    /** Write data to the file. */
    inline void Write(const DataObjects::String &data){
        Write(data.ConstData(), data.Length());
    }

    /** Read data from the file. */
    void Read(GBYTE *buffer, GUINT32 buf_len, GUINT32 bytes_to_read = UINT_MAX);
    /** Read data from the file and returns it in a string object. */
    DataObjects::String Read(GUINT32 bytes = UINT_MAX);

    /** Changes the position of the read pointer. */
    void Seek(GUINT32 pos);
    /** Returns the current position of the file pointer. */
    GUINT32 Pos() const;

    /** Satisfies the InputOutputInterface abstract interface. */
    virtual void WriteBytes(const GBYTE *data, GUINT32 len){
        Write(data, len);
    }

    /** Satisfies the InputOutputInterface abstract interface. */
    virtual void ReadBytes(GBYTE *buffer, GUINT32 buffer_len, GUINT32 bytes_to_read){
        Read(buffer, buffer_len, bytes_to_read);
    }
    
    
    /** Controls whether to buffer write outputs.  The default is true. 
        If there is a write buffer, then data is not written to the file immediately,
        but when it's convenient.  If there is no buffer, then data is written
        immediately.
    */
    PROPERTY(BufferedWrites, bool);


private:

    DataObjects::String m_filename;
    FILE *m_handle;

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_FILE_H
