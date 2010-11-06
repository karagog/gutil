/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "localsockettransport.h"
using namespace GUtil;

DataAccess::LocalSocketTransport::LocalSocketTransport(QLocalSocket *sock, QObject *parent)
    :GQIODevice(sock == 0 ? new QLocalSocket(this) : sock,
                                parent)
{
}

QLocalSocket &DataAccess::LocalSocketTransport::Socket() const
{
    return (QLocalSocket &)IODevice();
}

void DataAccess::LocalSocketTransport::ConnectToServer(const QString &server_name)
        throw(Core::DataTransportException)
{
    Socket().connectToServer(server_name);

    if(!Socket().waitForConnected(5000))
    {
        THROW_GUTIL_EXCEPTION( Core::DataTransportException(Socket().errorString().toStdString()) )
    }
}

void DataAccess::LocalSocketTransport::DisconnectFromServer()
{
    Socket().disconnectFromServer();
}

QString DataAccess::LocalSocketTransport::ServerName() const
{
    return Socket().serverName();
}
