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

#ifndef STREAMTRANSPORT_H
#define STREAMTRANSPORT_H

#include "abstractdatatransportmechanism.h"

namespace GUtil
{
    namespace DataAccess
    {
        namespace DataTransports
        {
            class StreamTransport : public AbstractDataTransportMechanism
            {
                Q_OBJECT
            public:
                explicit StreamTransport(std::istream *, std::ostream *, QObject *parent = 0);
                explicit StreamTransport(std::iostream *, QObject *parent = 0);


            protected:
                virtual void send_data(const QByteArray&) throw(GUtil::Core::DataTransportException);
                virtual QByteArray receive_data() throw(GUtil::Core::DataTransportException);

                virtual void update_has_data_variable(bool &has_data_variable) throw(GUtil::Core::DataTransportException);

                void SetIStreamPollingEnabled(bool);
                void SetIStreamPollingInterval(int new_interval = _default_polling_interval);

                virtual void timerEvent(QTimerEvent *);


            private:
                void _pre_init();
                void _post_init();

                std::istream *_stream_in;
                std::ostream *_stream_out;

                std::iostream *_stream_inout;

                std::ostream *get_ostream();
                std::istream *get_istream();

                int _timer_id;
                int _polling_interval;

                // By default we poll every 1 second, but you can change this by calling the functions above
                static const int _default_polling_interval = 1000;

            };
        }
    }
}

#endif // STREAMTRANSPORT_H
