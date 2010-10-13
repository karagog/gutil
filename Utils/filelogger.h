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

#ifndef FILELOGGER_H
#define FILELOGGER_H

#include "abstractstreamlogger.h"
#include <fstream>

namespace GUtil
{
    namespace Utils
    {
        class PubSubSystem;

        class FileLogger : public AbstractStreamLogger
        {
            Q_OBJECT
        public:
            explicit FileLogger(const QString &filename,
                                PubSubSystem *pss = 0,
                                QObject *parent = 0);

            // Clears the log file
            void ClearLog();

        protected:
            virtual std::ostream &Stream();

            virtual bool PreLogMessage();
            virtual void PostLogMessage();

        private:
            QString _filename;
            std::ofstream _fstream;

        };
    }
}

#endif // FILELOGGER_H
