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

#ifndef SIGNALWAITCONDITION_H
#define SIGNALWAITCONDITION_H

#include <QWaitCondition>
#include <QObject>

class QMutex;
class QReadWriteLock;

namespace GUtil
{
    namespace Custom
    {
        // Block your current thread of execution until the signal goes off.

        // Make sure not to wait on your own signal, 'cause you can't wake
        //  yourself up!
        class SignalWaitCondition :
                public QObject
        {
            Q_OBJECT
        public:
            explicit SignalWaitCondition(QObject *parent = 0);

            void WaitForSignal(QMutex *,
                    QObject *obj,
                    const char *signal,
                    unsigned long timeout = -1);

            void WaitForSignal(QReadWriteLock *,
                    QObject *obj,
                    const char *signal,
                    unsigned long timeout = -1);

        private slots:

            void _catch_signal();


        private:

            QWaitCondition _wait_condition;

        };
    }
}

#endif // SIGNALWAITCONDITION_H
