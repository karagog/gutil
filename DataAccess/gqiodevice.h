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

#ifndef GQIODEVICE_H
#define GQIODEVICE_H

#include "giodevice.h"
#include <QObject>

class QIODevice;

namespace GUtil
{
    namespace DataAccess
    {
        class GQIODevice :
                public GIODevice
        {
            Q_OBJECT
        signals:

            // Progress between 0 and 100.  Control the resolution between
            //  updates with the 'Write/ReadProgressResolution' member
            void WriteProgressUpdated(int);
            void ReadProgressUpdated(int);


        public:

            // The number of bytes to write in one chunk, before updating
            //  progress.  Set to -1 by default, which writes all in one chunk.
            PROPERTY( WriteProgressResolution, int );
            PROPERTY( ReadProgressResolution, int );

            READONLY_PROPERTY( WriteProgress, int );
            READONLY_PROPERTY( ReadProgress, int );

            virtual ~GQIODevice(){}


        protected:

            explicit GQIODevice(QIODevice *, QObject *parent = 0);

            inline QIODevice &IODevice(){ return *_io_device; }
            inline const QIODevice &IODevice() const{ return *_io_device; }

            virtual void send_data(const QByteArray &)
                    throw(GUtil::Core::DataTransportException);
            virtual QByteArray receive_data()
                    throw(GUtil::Core::DataTransportException);

            virtual bool has_data_available();


        private:

            void _fail_if_not_open();

            QIODevice *_io_device;

        };
    }
}

#endif // GQIODEVICE_H
