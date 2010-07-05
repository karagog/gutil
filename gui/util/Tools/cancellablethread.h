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

#ifndef ICANCELLABLETHREAD_H
#define ICANCELLABLETHREAD_H

#include <QThread>

namespace GQtUtil
{
    namespace Tools
    {
        // An interface for a thread that implements cancelling
        //  Note: You have to manually check if 'isCancelled()' is true and then
        //   exit by yourself.  That's why I say this is just an interface.
        class CancellableThread : public QThread
        {
            Q_OBJECT
        public:
            explicit CancellableThread(QObject *parent = 0);

            bool isCancelled();
            void uncancel();

        signals:
            void notifyCancelled();

        public slots:
            void cancel();


        private:
            bool is_cancelled;
        };
    }
}

#endif // ICANCELLABLETHREAD_H
