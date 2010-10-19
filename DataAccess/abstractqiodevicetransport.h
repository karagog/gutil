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

#ifndef IODEVICETRANSPORT_H
#define IODEVICETRANSPORT_H

#include "abstractdatatransportmechanism.h"
#include <QObject>

class QIODevice;

namespace GUtil
{
    namespace DataAccess
    {
        class AbstractQIODeviceTransport : public AbstractDataTransportMechanism
        {
            Q_OBJECT
        public:
            explicit AbstractQIODeviceTransport(QIODevice *, QObject *parent = 0);

        protected:
            QIODevice &IODevice() const;

            virtual void send_data(const QByteArray &) throw(GUtil::Core::DataTransportException);
            virtual QByteArray receive_data() throw(GUtil::Core::DataTransportException,
                                                    GUtil::Core::EndOfFileException);

            virtual void update_has_data_variable(bool &has_data_variable)
                    throw(GUtil::Core::DataTransportException);

        private:
            void _fail_if_not_open();

            QIODevice *_io_device;

        };
    }
}

#endif // IODEVICETRANSPORT_H
