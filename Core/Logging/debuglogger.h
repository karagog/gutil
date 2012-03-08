/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_DEBUGLOGGER_H
#define GUTIL_DEBUGLOGGER_H


// Use these defines to log (or not log) based on the preprocessor definition
#ifdef DEBUG_LOGGING

/** Logs a message to the debug logger (in debug mode only). */
#define dLogMessage( m )    GUtil::Logging::DebugLogger::LogMessage(__FILE__, __LINE__, m)

/** Logs a warning to the debug logger (in debug mode only). */
#define dLogWarning( m )    GUtil::Logging::DebugLogger::LogWarning(__FILE__, __LINE__, m)

/** Logs an error to the debug logger (in debug mode only). */
#define dLogError( m )      GUtil::Logging::DebugLogger::LogError(__FILE__, __LINE__, m)

/** Logs an exception to the debug logger (in debug mode only). */
#define dLogException( e )  GUtil::Logging::DebugLogger::LogException(e)

#else

/** Logs a message to the debug logger (in debug mode only). */
#define dLogMessage( m )

/** Logs a warning to the debug logger (in debug mode only). */
#define dLogWarning( m )

/** Logs an error to the debug logger (in debug mode only). */
#define dLogError( m )

/** Logs an exception to the debug logger (in debug mode only). */
#define dLogException( e )

#endif


#include "gutil_strings.h"
#include "globallogger.h"

NAMESPACE_GUTIL1(Logging);


/** A class to wrap debug logging functionality
    (DO NOT USE DIRECTLY, USE MACROS DEFINED ABOVE!)
*/
class DebugLogger
{
public:

    static void SetupLogFile(const DataObjects::String &fn);

    inline static void LogMessage(const char *file, int line,
                                  const DataObjects::String &msg)
    {
        _check_if_initialized();
        GlobalLogger::LogMessage(msg, _create_title(file, line), GlobalLogger::DebugId);
    }

    inline static void LogWarning(const char *file, int line,
                                  const DataObjects::String &msg)
    {
        _check_if_initialized();
        GlobalLogger::LogWarning( msg, _create_title(file, line), GlobalLogger::DebugId);
    }

    inline static void LogError(const char *file, int line,
                                const DataObjects::String &msg)
    {
        _check_if_initialized();
        GlobalLogger::LogError(msg, _create_title(file, line), GlobalLogger::DebugId);
    }

    inline static void LogException(const Exception<> &ex){
        _check_if_initialized();
        Logging::GlobalLogger::LogException(ex, GUtil::Logging::GlobalLogger::DebugId);
    }


private:

    static bool _initialized;

    inline static DataObjects::String _create_title(const char *file, int line){
        return DataObjects::String::Format("  File: %s    Line: %d", file, line);
    }

    static void _check_if_initialized();

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_DEBUGLOGGER_H
