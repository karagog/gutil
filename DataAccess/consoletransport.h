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

#ifndef CONSOLETRANSPORT_H
#define CONSOLETRANSPORT_H

#include "streamtransport.h"

namespace GUtil
{
    namespace DataAccess
    {
        class ConsoleTransport : public StreamTransport
        {
            Q_OBJECT
        public:
            explicit ConsoleTransport(QObject *parent = 0);

            static void WriteOut(const QString &);
        };
    }
}

#endif // CONSOLETRANSPORT_H
