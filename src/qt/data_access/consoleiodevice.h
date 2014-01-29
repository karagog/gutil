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

#ifndef GUTIL_CONSOLEIODEVICE_H
#define GUTIL_CONSOLEIODEVICE_H

#include "iodevice.h"
#include <QMutex>
#include <QQueue>

namespace GUtil{ namespace QT{ namespace DataAccess{


/** Implements an IODevice that listens and writes to the console on a 
    background thread.  It emits a signal asynchronously when data is ready
    on the console.  So you can use this class as an asynchronous
    Human Interface Device.
    
    You may only instantiate one of these per process.  Trying to instantiate
    a second one will throw an exception from the constructor.
*/
class ConsoleIODevice :
        public IODevice
{
    Q_OBJECT
public:

    /** Constructor for the ConsoleIODevice.  This will throw an exception if
        you have already instantiated one.
    */
    ConsoleIODevice();
    ~ConsoleIODevice();


public slots:

    /** Writes a line of data asynchronously to the console.  That means that the
        function returns immediately after queuing the data to be written on
        the background thread.
    */
    void WriteLine(const QString &);
    
    /** Reads a line from the console. */
    QString ReadLine();


protected:

    // Just reads/writes to stdin/out
    virtual void send_data(const QByteArray &);
    virtual QByteArray receive_data();

    bool has_data_available();

    /** We continually read cin on a separate thread.
        This thread's code is inside here
    */
    virtual void run();

};


}}}

#endif // GUTIL_CONSOLEIODEVICE_H
