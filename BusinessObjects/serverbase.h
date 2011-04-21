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

#ifndef SERVERBASE_H
#define SERVERBASE_H

#include "connectionmanager.h"

namespace GUtil{ namespace BusinessObjects{


// Used as a base class for any kind of server
class ServerBase :
        public ConnectionManager
{
    Q_OBJECT
public:

    explicit ServerBase(QObject *parent = 0);


signals:

    void NewConnection(const QUuid &conn_id = QUuid());
    void ClientDisconnected(const QUuid &conn_id = QUuid());


protected slots:

    void client_disconnected(const QUuid &);

};


}}

#endif // SERVERBASE_H

#endif // NETWORK_FUNCTIONALITY
