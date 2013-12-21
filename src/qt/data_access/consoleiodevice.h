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


class ConsoleIODevice :
        public IODevice
{
    Q_OBJECT
public:

    explicit ConsoleIODevice(QObject *parent = 0);
    virtual ~ConsoleIODevice();

    void Engage(){ SetEngaged(true); }
    void Disengage(){ SetEngaged(false); }

    bool IsEngaged() const{ return _engaged; }


public slots:

    void WriteLine(const QString &);
    QString ReadLine();

    /** Stops/starts the object from listening to cin/cout */
    void SetEngaged(bool);


protected:

    // Just reads/writes to stdin/out
    virtual void send_data(const QByteArray &);
    virtual QByteArray receive_data();

    bool has_data_available();

    /** We continually read cin on a separate thread.
        This thread's code is inside here
    */
    virtual void run();


private:

    QQueue<QString> _messages_received;
    QMutex _messages_lock;

    // Only one of these objects can interface with the console
    static QMutex global_console_mutex;

    bool _engaged;
    void _fail_if_not_initialized();

    bool _has_data_available_locked() const;

};


}}}

#endif // GUTIL_CONSOLEIODEVICE_H
