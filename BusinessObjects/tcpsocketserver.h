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

#ifdef NETWORK_FUNCTIONALITY

#ifndef TCPSOCKETSERVER_H
#define TCPSOCKETSERVER_H

#include "serverbase.h"
#include <QTcpServer>
#include <QHostAddress>


namespace GUtil{
namespace DataAccess{
    class GTcpSocketIODevice;
}

namespace BusinessObjects{


class TcpSocketServer :
        public ServerBase
{
    Q_OBJECT
public:

    explicit TcpSocketServer(QObject *parent = 0);
    ~TcpSocketServer();

    // Throws an exception if listen fails
    void ListenForConnections(const QHostAddress &address = QHostAddress::Any,
                              quint16 port = 0);

    // Close all connections and stop listening for new ones
    void ShutDownServer();

    // The host address/port we're listening on
    inline QHostAddress Address() const{
        return _server.serverAddress();
    }
    inline quint16 Port() const{
        return _server.serverPort();
    }


private slots:

    void new_connection();


private:

    QTcpServer _server;

};


}}


#endif // TCPSOCKETSERVER_H

#endif // NETWORK_FUNCTIONALITY
