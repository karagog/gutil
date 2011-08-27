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

#if defined(NETWORK_FUNCTIONALITY)

#ifndef GUTIL_PROCESSSTATUSSERVER_H
#define GUTIL_PROCESSSTATUSSERVER_H

#include <QLocalServer>
#include <QThreadPool>

namespace GUtil{ namespace BusinessObjects{


class ProcessStatusServer :
        public QLocalServer
{
    Q_OBJECT
public:

    explicit ProcessStatusServer(QObject *p = 0);


    /** Wait for any remaining threads to finish. */
    ~ProcessStatusServer();

    void NotifyNewMessage(const QByteArray &);

signals:

    void NewMessage(QByteArray);


private:

    void incomingConnection(quintptr socketDescriptor);
    QThreadPool thread_pool;

};


// This implementation is supposed to be private.
class ProcessThread :
        public QRunnable
{
public:
    explicit ProcessThread(ProcessStatusServer *server, quintptr socket_descriptor);
private:
    void run();
    quintptr _sd;
    ProcessStatusServer *_server;
};


}}


#endif // GUTIL_PROCESSSTATUSSERVER_H

#endif // NETWORK_FUNCTIONALITY
