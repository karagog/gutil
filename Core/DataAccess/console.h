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

#ifndef GUTIL_CONSOLE_H
#define GUTIL_CONSOLE_H

#include "iointerface.h"
#include "Core/DataObjects/gstring.h"

NAMESPACE_GUTIL1(DataAccess);


/** A blocking IO device that reads/writes to the console.

    Normally you can just use the static functions to write/read from the console,
    but it also has the feature that it implements InputOutputInterface, so you can
    instantiate one and use it like an IO device.
*/
class Console :
        public InputOutputInterface
{
public:

    /** Differentiates between the output streams 1 and 2 (standard out and error). */
    enum OutputStreamEnum
    {
        /** The default output stream. */
        StandardOut,
        StandardError
    };

    /** Constructs a default Console object, which outputs to standard out. */
    inline Console() :_p_OutputStream(StandardOut){}
    /** Constructs a Console object with your own output specification. */
    inline Console(OutputStreamEnum e) : _p_OutputStream(e){}

    /** Determines which output stream to which we will output*/
    PROPERTY( OutputStream, OutputStreamEnum );


    /** Writes to the console */
    static void Write(const char *, OutputStreamEnum e = StandardOut);
    /** Writes a line to the console.  If the data consists of multiple lines
        then they are all written and the last is followed by a new line.
    */
    static void WriteLine(const char *d = 0, OutputStreamEnum e = StandardOut);


    /** Reads from the console into the buffer.
        If you pass UINT_MAX as a byte count, then it will read until the first
        whitespace character
        \returns The number of bytes it read into the buffer
    */
    static GUINT32 Read(GBYTE *buffer, GUINT32 buffer_len, GUINT32 bytes = UINT_MAX);

    /** Reads from the console. */
    inline static DataObjects::String Read(GUINT32 bytes){
        DataObjects::String ret(bytes);
        ret.Resize(bytes);
        Read(reinterpret_cast<GBYTE *>(ret.Data()), bytes, bytes);
        return ret;
    }
    /** Reads a line from the console. */
    static DataObjects::String ReadLine(){
        return _read_until_terminator(&_is_end_line);
    }

    /** Reads an integer from stdin */
    static GINT32 ReadInteger();

    /** Reads a floating point number from stdin */
    static GFLOAT32 ReadFloat();

    /** Reads a string from stdin (reads until a whitespace character) */
    inline static DataObjects::String Read(){
        return _read_until_terminator(&_is_whitespace);
    }



    /** Satisfies the InputOutputInterface abstract interface.  The second argument is ignored. */
    virtual void WriteBytes(const GBYTE *data, GUINT32){
        Write(reinterpret_cast<const char *>(data), GetOutputStream());
    }
    /** Satisfies the InputOutputInterface abstract interface.  The second argument is ignored. */
    virtual void ReadBytes(GBYTE *buffer, GUINT32 buffer_len, GUINT32 bytes_to_read){
        Read(buffer, buffer_len, bytes_to_read);
    }


private:

    static DataObjects::String _read_until_terminator(bool (*is_terminator_char)(char));

    static bool _is_end_line(char c);
    static bool _is_whitespace(char c);

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_CONSOLE_H
