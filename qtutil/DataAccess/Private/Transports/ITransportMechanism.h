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
#include <QMutex>

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

                    // These functions are atomic with respect to each other;
                    //  feel free to call these in multiple threads
                    void sendData(const QByteArray&);
                    QByteArray receiveData();

                protected:

                    // Derived classes must implement these functions
                    //  Note that locking is taken care of by this interface class,
                    //  so you can trust that these are atomic WRT each other
                    virtual void send_data(const QByteArray&) = 0;
                    virtual QByteArray receive_data() = 0;

                private:

                    // Protects us so we can be re-entrant when using the transport
                    QMutex _lock;

                };
            }
        }
    }
}

#endif // ITRANSPORTMECHANISM_H
