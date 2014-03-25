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

/** Throw this exception from the ReadBytes and WriteBytes functions, and
 *  provide the correct number of bytes
*/
GUTIL_EXCEPTION_DECLARE_WITH_MEMBERS( IOException, int Bytes );


/** An abstract interface that says you implement a way of receiving data
    from somewhere
*/
class InputInterface
{
public:
    /** Attempts to read data from the device.

        Derived classes should throw an IOException upon error, with the
        number of bytes read.

        If no exception is thrown, it should return the number of bytes read.

        \param buffer A buffer into which to read data
        \param buffer_len The length (in bytes) of the buffer
        \param bytes_to_read The number of bytes you want to read.  You can
        pass UINT_MAX to have it read all available data.
    */
    virtual GUINT32 ReadBytes(GBYTE *buffer,
                           GUINT32 buffer_len,
                           GUINT32 bytes_to_read = UINT_MAX) = 0;

    /** Should return the number of bytes available to be read.
        Classes can override the base implementation, which always return -1.
    */
    virtual GUINT32 BytesAvailable() const{ return -1; }

    virtual ~InputInterface(){}
};


/** An abstract interface that says you implement a way of outputting
    data somewhere
*/
class OutputInterface
{
public:
    /** Writes the data to the IO device.

        Derived classes should throw an exception upon error, with the number
        of bytes written.

        If no exception is thrown, it should return the number of bytes written.
    */
    virtual GUINT32 WriteBytes(const GBYTE *data, GUINT32 len) = 0;
    virtual ~OutputInterface(){}
};


/** An abstract interface that says you implement a way of receiving and
    producing byte data.
*/
class InputOutputInterface :
        public InputInterface,
        public OutputInterface
{
public:
    virtual ~InputOutputInterface(){}
};


}

#endif // GUTIL_IOINTERFACE_H
