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

#ifndef GSOCKETIODEVICE_H
#define GSOCKETIODEVICE_H

#include "gqiodevice.h"

class QLocalSocket;

namespace GUtil
{
    namespace DataAccess
    {
        class GSocketIODevice :
                public GQIODevice
        {
            Q_OBJECT
        public:

            explicit GSocketIODevice(QLocalSocket *,
                                     QObject *parent = 0);

            bool IsConnected() const;

            inline QLocalSocket &Socket(){
                return (QLocalSocket &)IODevice();
            }
            inline const QLocalSocket &Socket() const{
                return (const QLocalSocket &)IODevice();
            }

        };
    }
}

#endif // GSOCKETIODEVICE_H
