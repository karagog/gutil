/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifdef GUI_FUNCTIONALITY

#ifndef DEBUGLOGGER_H
#define DEBUGLOGGER_H


// Use these defines to log (or not log) based on the preprocessor definition
#ifdef DEBUG_LOGGING

#define dSetupLogFile( fn ) GUtil::Logging::DebugLogger::SetupLogFile(fn)
#define dClearLog()         GUtil::Logging::DebugLogger::ClearLog()

#define dLogVariable( v )   GUtil::Logging::DebugLogger::LogVariable(__FILE__, __LINE__, #v, v)

#define dLogMessage( m )    GUtil::Logging::DebugLogger::LogMessage(__FILE__, __LINE__, m)
#define dLogWarning( m )    GUtil::Logging::DebugLogger::LogWarning(__FILE__, __LINE__, m)
#define dLogError( m )      GUtil::Logging::DebugLogger::LogError(__FILE__, __LINE__, m)

#define dLogException( e )  GUtil::Logging::DebugLogger::LogException(e)

#else

#define dSetupLogFile( fn )
#define dClearLog()

#define dLogVariable( v )

#define dLogMessage( m )
#define dLogWarning( m )
#define dLogError( m )

#define dLogException( e )

#endif




#include "globallogger.h"
#include "filelogger.h"
#include <QVariant>

#define DEFAULT_LOG_FILENAME "debug.log"

namespace GUtil
{
    namespace Logging
    {
        // A class to wrap debug logging functionality (DO NOT USE DIRECTLY, USE MACROS DEFINED ABOVE!)
        class DebugLogger
        {
        public:
            static void SetupLogFile(const QString &fn);
            static void ClearLog();

            static void LogVariable(const char *file, int line,
                                    const QString &var_name, const QVariant &var);
            static void LogMessage(const char *file, int line,
                                   const QString &msg);
            static void LogWarning(const char *file, int line,
                                   const QString &msg);
            static void LogError(const char *file, int line,
                                 const QString &msg);

            static void LogException(const GUtil::Core::Exception<> &ex);
            static void LogException(const std::exception &ex);


        private:

            static bool _initialized;

            static QString _create_title(const char *file, int line);
            static void _check_if_initialized();

        };
    }
}

#endif // DEBUGLOGGER_H

#endif // GUI_FUNCTIONALITY
