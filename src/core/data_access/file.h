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

#ifndef GUTIL_FILE_H
#define GUTIL_FILE_H

#include "gutil_iio.h"
#include "gutil_strings.h"
#include <functional>

NAMESPACE_GUTIL;


/** An encapsulation of a file, that gives easy access to file operations.

    This class is exception driven, so you should expect exceptions in
    failure cases and trap them.
*/
class File :
        public IRandomAccessInput,
        public IOutput
{
    GUTIL_DISABLE_COPY(File);
public:

    /** Creates a file object with the given filename. */
    explicit File(const char *filename);
    virtual ~File();

    /** Returns the current filename we're set to. */
    const char *Filename() const{ return m_filename; }

    /** Returns whether the file exists. */
    bool Exists() const{ return Exists(m_filename); }
    /** Returns whether the file exists. */
    static bool Exists(const char *filename);

    /** Creates the file if it doesn't exist. If it does exist it updates the modification
     *  time but leaves the file otherwise unchanged.
    */
    static void Touch(const char *filename);

    /** Creates the file if it doesn't exist. If it does exist it updates the modification
     *  time but leaves the file otherwise unchanged.
    */
    inline void Touch(){ Touch(m_filename); }

    /** Closes the file and deletes it. */
    void Delete(){
        if(IsOpen()) Close();
        Delete(m_filename);
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
    bool IsOpen() const{ return h; }

    /** Closes the file if it is open. */
    void Close();

    /** The length of the file. */
    virtual GUINT64 Length() const;
    /** The length of the file. */
    GUINT64 Size() const{ return Length(); }

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

    /** Flushes the file's output buffer.
        \sa SetBufferedWrites()
    */
    virtual void Flush();

    /** Read data from the file.  Return the number of bytes read. */
    GUINT32 Read(GBYTE *buffer, GUINT32 buf_len, GUINT32 bytes_to_read = UINT_MAX);
    /** Read data from the file and returns it in a string object. */
    String Read(GUINT32 bytes = UINT_MAX);
    /** Reads one byte at a time until the predicate returns true.
        After each read the predicate will be called with the buffer that has
        accumulated all bytes until that point
        \param pred A function that tells when to stop reading. It returns true when
            you want to stop. It is passed a reference to the string, and the integer
            that is non-zero if you're at the end of the file.
    */
    String ReadUntil(std::function<bool(String &)> pred);

    /** Reads the file until the next new line character.
        \param max_len The maximum number of bytes to read. This prevents you from reading
            forever if the file has no new lines. By default it is not limited.
        \returns A null string if there is no more data in the file,
            or an empty string if the next line is empty. Otherwise
            it returns the next line in the file, without the new line separator.
    */
    String ReadLine(GUINT32 max_len = GUINT32_MAX);

    /** Changes the position of the read pointer. */
    virtual void Seek(GUINT64 pos);
    /** Returns the current position of the file pointer. */
    virtual GUINT64 Pos() const;

    /** Satisfies the IInput and IOutput abstract interface. */
    virtual GUINT32 WriteBytes(const GBYTE *data, GUINT32 len){
        Write(data, len);
        return len;
    }

    /** Satisfies the IInput abstract interface. */
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

    char *m_filename;
    void *h;

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_FILE_H
