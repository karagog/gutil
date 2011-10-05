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

#include "processstatusserver.h"
#include "gutil.h"
#include <QLocalSocket>
GUTIL_USING_NAMESPACE(BusinessObjects);

ProcessStatusServer::ProcessStatusServer(QObject *p)
    :QLocalServer(p)
{}

ProcessStatusServer::~ProcessStatusServer()
{
    thread_pool.waitForDone();
}

void ProcessStatusServer::incomingConnection(quintptr socketDescriptor)
{
    thread_pool.start(new ProcessThread(this, socketDescriptor));
}

void ProcessStatusServer::NotifyNewMessage(const QByteArray &ba)
{
    emit NewMessage(ba);
}



ProcessThread::ProcessThread(ProcessStatusServer *server, quintptr socket_descriptor)
    :_sd(socket_descriptor),
      _server(server)
{

}


// The server thread simply accepts one message.  It will stop waiting
//  for this message after a predetermined amount of time
#define SERVER_WAIT_TIME 5000

void ProcessThread::run()
{
    QLocalSocket sock;
    sock.setSocketDescriptor(_sd);

    if(sock.waitForReadyRead(SERVER_WAIT_TIME))
        _server->NotifyNewMessage(sock.readAll());
}
