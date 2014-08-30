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

#ifndef GUTIL_SERIAL_H
#define GUTIL_SERIAL_H

#ifdef _WIN32
        #include <windows.h>
#endif

#include "gutil_iointerface.h"

/** \file

    This is the header for cross-platform serial port functionality
*/

NAMESPACE_GUTIL;


/** A cross-platform serial port class.  Writes and reads bytes to/from a serial port.
    It currently works in Windows and Linux only.
*/
class SerialPort :
        public InputInterface,
        public OutputInterface
{
public:

    /** Creates an empty serial port instance. You must call Initialize before use. */
    SerialPort();
    /** Closes the serial port */
    ~SerialPort();

    /** Open the port, Setup the BAUD rate and such.
        You must call this function before using this class.
    */
    void Initialize(const char *com_port);

    /** Returns whether the serial port is currently open*/
    bool IsOpen() const{ return m_handle != 0; }
    /** Closes the open serial port.  Does nothing if it's not open. */
    void Close();

    /** Returns 0 on successful write */
    virtual GUINT32 WriteBytes(const GBYTE *, GUINT32 len);
    /** Returns 0 on successful read */
    virtual GUINT32 ReadBytes(GBYTE *buf, GUINT32 buf_len, GUINT32 to_read);


private:
#ifdef linux
    int m_handle;
#else
    HANDLE m_handle;
#endif
};


END_NAMESPACE_GUTIL;

#endif  // GUTIL_SERIAL_H
