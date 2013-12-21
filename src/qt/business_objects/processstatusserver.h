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

#ifndef GUTIL_NO_NETWORK_FUNCTIONALITY

#ifndef GUTIL_PROCESSSTATUSSERVER_H
#define GUTIL_PROCESSSTATUSSERVER_H

#include "gutil_smartpointer.h"
#include <QLocalSocket>
#include <QLocalServer>
#include <QThreadPool>

namespace GUtil{ namespace QT{ namespace BusinessObjects{




/** Implements an application status server, which provides status information
    to sub-process which query it.
*/
class ProcessStatusServer :
        public QLocalServer
{
    Q_OBJECT
public:

    /** Creates a new server object */
    explicit ProcessStatusServer(QObject *p = 0) :QLocalServer(p){}

    /** Destructor waits for any remaining connections to close */
    ~ProcessStatusServer(){ thread_pool.waitForDone(); }

    /** Used to notify that a new message was received.  This is called by our
        background thread; you shouldn't have to use it manually.
    */
    void PublishNewMessage(const QByteArray &ba){ emit NewMessage(ba); }


signals:

    /** Notifies that a new message was received (either on the socket or manually
        via PublishNewMessage()
    */
    void NewMessage(QByteArray);


private:

    virtual void incomingConnection(quintptr socketDescriptor);

    QThreadPool thread_pool;
    GUtil::Utils::SmartPointer<QRunnable> _thread;

};


/** This is a private implementation for the ProcessStatusServer use only */
class ProcessStatusServer_Thread :
        public QRunnable
{
public:
    explicit ProcessStatusServer_Thread(ProcessStatusServer *server,
                                               quintptr socket_descriptor)
        :_sd(socket_descriptor),
          _server(server)
    { setAutoDelete(false); }
    virtual void run(){
        QLocalSocket sock;
        sock.setSocketDescriptor(_sd);

        if(sock.waitForReadyRead(5000))
            _server->PublishNewMessage(sock.readAll());
    }
private:
    quintptr _sd;
    ProcessStatusServer *_server;
};


}}}


#endif // GUTIL_PROCESSSTATUSSERVER_H

#endif // GUTIL_NO_NETWORK_FUNCTIONALITY
