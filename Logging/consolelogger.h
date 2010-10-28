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

#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "abstractlogger.h"
#include "DataAccess/gconsoleiodevice.h"
#include <QObject>

namespace GUtil
{
    namespace Logging
    {
        // Logs to the console
        class ConsoleLogger : public AbstractLogger
        {
        protected:
            virtual void Log_protected(const QByteArray &, MessageLevelEnum);

        private:
            DataAccess::GConsoleIODevice _my_console_io_device;

        };
    }
}

#endif // CONSOLELOGGER_H
