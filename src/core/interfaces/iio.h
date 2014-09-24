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

#ifndef GUTIL_IOINTERFACE_H
#define GUTIL_IOINTERFACE_H

#include "gutil_exception.h"
#include <limits.h>

namespace GUtil{


/** An abstract interface that says you implement a way of receiving data
    from somewhere
*/
class IInput
{
public:
    /** Attempts to read data from the device.

        If no exception is thrown, it should return the number of bytes read.

        \param buffer A buffer into which to read data
        \param buffer_len The length (in bytes) of the buffer
        \param bytes_to_read The number of bytes you want to read.  You can
        pass UINT_MAX to have it read all available data.
    */
    virtual GUINT32 ReadBytes(GBYTE *buffer,
                              GUINT32 buffer_len,
                              GUINT32 bytes_to_read = UINT_MAX) = 0;

    /** Returns the number of bytes available to be read.
     *  If it doesn't know how many bytes there are available (as with a
     *  pipe or network connection), it should return UINT_MAX.
     *  \returns 0 if the source is exhausted, int64_max if it doesn't know,
     *      otherwise returns the number of bytes that can still be read.
    */
    virtual GUINT64 BytesAvailable() const = 0;

    virtual ~IInput(){}
};


/** An abstract interface that says you implement a way of outputting
    data somewhere
*/
class IOutput
{
public:
    /** Writes the data to the IO device.

        Derived classes should throw an exception upon error, with the number
        of bytes written.

        If no exception is thrown, it should return the number of bytes written.
    */
    virtual GUINT32 WriteBytes(const GBYTE *data, GUINT32 len) = 0;

    /** Flushes the output buffer, if there is one. Otherwise does nothing. */
    virtual void Flush(){}

    virtual ~IOutput(){}
};


class IRandomAccessInput :
        public IInput
{
public:

    /** Returns the total size of the input from beginning to end. */
    virtual GUINT64 Length() const = 0;

    /** Returns the current position of the input. */
    virtual GUINT64 Pos() const = 0;

    /** Moves the current position to the given value. */
    virtual void Seek(GUINT64) = 0;

    /** Provides a default implementation
     *  which should should work for most cases.
    */
    virtual GUINT64 BytesAvailable() const{ return this->Length() - this->Pos(); }

    virtual ~IRandomAccessInput(){}
};


}

#endif // GUTIL_IOINTERFACE_H
