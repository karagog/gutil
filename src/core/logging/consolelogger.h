/*Copyright 2010-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_CONSOLELOGGER_H
#define GUTIL_CONSOLELOGGER_H

#include <gutil/outputdevicelogger.h>
#include <gutil/console.h>

NAMESPACE_GUTIL;


/** A logger that outputs to the console. */
class ConsoleLogger : public OutputDeviceLogger
{
public:
    inline ConsoleLogger(Console::OutputStreamEnum e = Console::StandardOut)
        :OutputDeviceLogger(new Console(e)) {}
};


END_NAMESPACE_GUTIL;

#endif // GUTIL_CONSOLELOGGER_H
