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

#ifndef PUBSUBSYSTEM_H
#define PUBSUBSYSTEM_H

#include <QObject>

namespace GUtil
{
    namespace Utils
    {
        class PubSubSystem : public QObject
        {
            Q_OBJECT
        public:
            explicit PubSubSystem(QObject *parent = 0);


        public slots:
            void PublishMessage(const QByteArray &);


        signals:
            void NotifyMessage(const QByteArray &);


        protected:

            // Derived classes can override this function to do other interesting things
            //   with the published message
            virtual void on_message_published(const QByteArray &);


        };
    }
}

#endif // PUBSUBSYSTEM_H