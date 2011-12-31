/*Copyright 2011 George Karagoulis

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

#include "Core/exception.h"
#include <limits.h>
NAMESPACE_GUTIL1(DataAccess);


/** An abstract interface that says you implement a way of receiving data
    from somewhere
*/
class InputInterface
{
public:
    /** Attempts to read data from the device.

        Derived classes should throw an exception upon error

        \param buffer A buffer into which to read data
        \param buffer_len The length (in bytes) of the buffer
        \param bytes_to_read The number of bytes you want to read.  You can
        pass UINT_MAX to have it read all available data.
    */
    virtual void ReadBytes(GBYTE *buffer,
                           GUINT32 buffer_len,
                           GUINT32 bytes_to_read = UINT_MAX) = 0;

    /** Should return the number of bytes available to be read.
        Classes can override the base implementation, which always return -1.
    */
    virtual int BytesAvailable() const{ return -1; }

    virtual ~InputInterface(){}
};


/** An abstract interface that says you implement a way of outputting
    data somewhere
*/
class OutputInterface
{
public:
    /** Writes the data to the IO device.

        Derived classes should throw an exception upon error, and include whatever
        information you want to therein.
    */
    virtual void WriteBytes(const GBYTE *data, GUINT32 len) = 0;
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


END_NAMESPACE_GUTIL1;

#endif // GUTIL_IOINTERFACE_H
