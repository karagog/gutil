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

#ifndef LOCALSOCKETCLIENT_H
#define LOCALSOCKETCLIENT_H

#include "clientbase.h"
#include <QObject>

namespace GUtil
{
    namespace BusinessObjects
    {
        class LocalSocketClient :
                public ClientBase
        {
            Q_OBJECT
        public:

            explicit LocalSocketClient(QObject *parent = 0);

            void ConnectToServer(const QString &identifier,
                                 const QString &modifier = QString::null);

            void DisconnectFromServer();

        };
    }
}

#endif // LOCALSOCKETCLIENT_H

#endif // NETWORK_FUNCTIONALITY
