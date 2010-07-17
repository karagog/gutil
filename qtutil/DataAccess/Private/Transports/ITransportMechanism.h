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

#ifndef ITRANSPORTMECHANISM_H
#define ITRANSPORTMECHANISM_H

#include <QByteArray>

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            namespace Transports
            {
                // This is an abstract base class to represent different methods of data transport
                class ITransportMechanism
                {
                public:
                    virtual void exportData(const QByteArray&) = 0;
                    virtual QByteArray importData() = 0;
                };
            }
        }
    }
}

#endif // ITRANSPORTMECHANISM_H
