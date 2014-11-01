/*Copyright 2010-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_CONSOLEIODEVICE_H
#define GUTIL_CONSOLEIODEVICE_H

#include "iodevice.h"

namespace GUtil{ namespace Qt{


/** Implements an IODevice that listens and writes to the console on a 
    background thread.  It emits a signal asynchronously when data is ready
    on the console.  So you can use this class as an asynchronous
    Human Interface Device.

    This is not a thread-safe class.  It manages its own background threads, but you must still
    implement your own locking if you plan to use this from multiple threads.
    
    You may only instantiate one of these at a time per application.  Trying to instantiate
    a second one will throw an exception from the constructor.
*/
class ConsoleIODevice :
        public IODevice
{
    Q_OBJECT
public:

    /** Constructor for the ConsoleIODevice.  This will throw an exception if
        you have already instantiated one at the time.

        \param asynchronous_write Initializes the AsynchronousWrite property
            to the given value.
    */
    explicit ConsoleIODevice(bool asynchronous_write = true, QObject * = 0);
    virtual ~ConsoleIODevice();

    /** Controls whether or not we write to console asynchronously.
     *
     *  If this is true then calls to WriteLine() return immediately after queuing
     *  the data for write, but they don't wait for the data to be written.
     *
     *  If this is false then this class will block on writes until the given data
     *  is written to the console.
     *
     *  True by default.
    */
    PROPERTY(AsynchronousWrite, bool);

    /** Reads a line from the console. */
    QString ReadLine();

    /** \name Synchronization Functions
     *  \{
    */

    /** Waits for data to be ready for reading.
     *  \param wait_time The number of milliseconds to wait.
     *  \returns true if there is data ready
    */
    bool WaitForReadyRead(unsigned long wait_time = ULONG_MAX) const;

    /** Waits for the line denoted by ID to be written.  There is no need to
     *   pass a wait time or return value because there is no chance of a timeout. The
     *   line definitely gets written at sometime, unlike waiting for user input, in
     *   which case the user may never give you anything.
     *
     *  \param ID The line id returned from WriteLine()
    */
    void WaitForLineWritten(GUINT64 ID) const;

    /** \} */


public slots:

    /** Writes a line of data asynchronously to the console.  That means that the
        function returns immediately after queuing the data to be written on
        the background thread.

        \returns A line id that can be used for synchronization.  You can wait until
        any given line is written to console with WaitForLineWritten().
    */
    GUINT64 WriteLine(QString);


protected:

    /** \name IODevice Interface
     *  \{
    */
    virtual void send_data(const QByteArray &);
    virtual QByteArray receive_data();
    virtual bool has_data_available();
    /** \} */

};


}}

#endif // GUTIL_CONSOLEIODEVICE_H
