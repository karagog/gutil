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
        // This class is reentrant, so feel free to access it from multiple threads

        // It holds QVariant data in case you want to store some state inside it
        class PubSubSystem : public QObject
        {
            Q_OBJECT
        public:
            explicit PubSubSystem(QObject *parent = 0);

        signals:
            void NotifyMessage(QString message, QString title);
            void NotifyWarning(QString message, QString title);
            void NotifyError(QString message, QString title);

            void NotifyProgress(int progress, int id);

        public slots:
            virtual void PublishMessage(const QString& message, const QString& title = QString::null);
            virtual void PublishWarning(const QString& message, const QString& title = QString::null);
            virtual void PublishError(const QString& message, const QString& title = QString::null);

            virtual void PublishProgress(int progress, int id = -1);

        };
    }
}

#endif // PUBSUBSYSTEM_H
