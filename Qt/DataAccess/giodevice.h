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

#ifndef GIODEVICE_H
#define GIODEVICE_H

#include "Core/Interfaces/ireadonlyobject.h"
#include "Core/extendedexception.h"
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <QUuid>
#include <QMetaType>

namespace GUtil{ namespace QT{ namespace DataAccess{


/** This is an abstract base class to represent different methods of data transport

    The interface to the outside world is limited to two functions:
    sendData(QByteArray) - SLOT
    notifyNewData(QByteArray) - SIGNAL

    These functions simplify data transportation to send/receive commands.  The derived transport class
    will be responsible for the actual transport mechanism, including protocols and the data exchange.

    This is a thread-safe class.  You can call any of the public functions from different threads
    and the class implements mutex locking for you.
*/

class GIODevice :

        // This derives from QThread to enable you to use the thread if you want to,
        //  but the base implementation does not make use of the thread
        public QThread,
        public GUtil::Interfaces::IReadOnlyObject
{
    Q_OBJECT
public slots:

    void SendData(const QByteArray &);
    inline void Write(const QByteArray &data){ SendData(data); }


signals:

    // This signal happens when there's new data available
    void ReadyRead(const QUuid &identity_of_emitter = QUuid());


public:

    virtual ~GIODevice();

    bool HasDataAvailable();

    // ReadyRead() will tell you when you can get new data
    QByteArray ReceiveData();

    READONLY_PROPERTY( Identity, QUuid );

    GIODevice &operator << (const char*);
    GIODevice &operator << (const QString &);
    GIODevice &operator << (const QByteArray &);
    void operator >> (QByteArray &);
    void operator >> (QString &);


protected:

    explicit GIODevice(QObject *parent = 0);

    // Derived classes implement a method to determine if data is available
    virtual bool has_data_available() = 0;

    // Derived classes must implement these functions
    //  Note that locking is taken care of by this interface class,
    //  so you can trust that these are atomic WRT each other
    virtual void send_data(const QByteArray&) = 0;
    virtual QByteArray receive_data() = 0;

    // IReadonly interface
    virtual const char *ReadonlyMessageIdentifier() const{ return "GUtil::QT::DataAccess::GIODevice"; }


protected slots:

    // This emits the readyRead signal
    void raiseReadyRead();


private:

    // Protects us so we can be thread-safe
    QMutex _this_lock;

};


}}}

#endif // GIODEVICE_H
