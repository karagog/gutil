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
#include <QMap>
#include <QVariant>
#include <QReadWriteLock>

namespace GQtUtil
{
    namespace Utils
    {
        // This class is reentrant, so feel free to access it from multiple threads

        class PubSubSystem : public QObject
        {
            Q_OBJECT
        public:
            explicit PubSubSystem(QObject *parent = 0);

            QVariant getData(int id);

        signals:
            void notifyMessage(QString title, QString message);
            void notifyProgress(int progress, int id);

        public slots:
            void publishMessage(QString title, QString message);
            void publishProgress(int progress, int id = -1);

            void setData(int, const QVariant &);

        private:
            QReadWriteLock data_lock;
            QMap<int, QVariant> _data;

        };
    }
}

#endif // PUBSUBSYSTEM_H
