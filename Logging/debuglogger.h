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

#ifndef DEBUGLOGGER_H
#define DEBUGLOGGER_H

#ifdef DEBUG_LOGGING

#define DEFAULT_LOG_FILENAME "debug.log"


#include <QVariant>
#include "globallogger.h"
#include "filelogger.h"

// Use these defines to log (or not log) based on the preprocessor definition
#define dSetupLogFile( fn ) DebugLogger::SetupLogFile(fn)
#define dClearLog()         DebugLogger::ClearLog()

#define dLogVariable( v )   DebugLogger::LogVariable(__FILE__, __LINE__, #v, v)

#define dLogMessage( m )    DebugLogger::LogMessage(__FILE__, __LINE__, m)
#define dLogWarning( m )    DebugLogger::LogWarning(__FILE__, __LINE__, m)
#define dLogError( m )      DebugLogger::LogError(__FILE__, __LINE__, m)

#define dLogException( e )  DebugLogger::LogException(e)



// A class to wrap debug logging functionality (DO NOT USE DIRECTLY, USE MACROS DEFINED ABOVE!)
class DebugLogger
{
public:
    static void SetupLogFile(const QString &fn){
        GUtil::Logging::GlobalLogger::SetupLogger(
                new GUtil::Logging::FileLogger(fn),
                GUtil::Logging::GlobalLogger::DebugId);

        _initialized = true;

        ClearLog();
    }

    static void ClearLog(){
        _check_if_initialized();

        GUtil::Logging::GlobalLogger::ClearLog(GUtil::Logging::GlobalLogger::DebugId);
    }

    static void LogVariable(const char *file, int line, const QString &var_name, const QVariant &var){
        _check_if_initialized();

        QString msg = QString("Variable '%1'    Value: '%2'").arg(var_name).arg(var.toString());

        GUtil::Logging::GlobalLogger::LogMessage(
                msg,
                _create_title(file, line),
                GUtil::Logging::GlobalLogger::DebugId);
    }

    static void LogMessage(const char *file, int line, const QString &msg){
        _check_if_initialized();

        GUtil::Logging::GlobalLogger::LogMessage(
                msg,
                _create_title(file, line),
                GUtil::Logging::GlobalLogger::DebugId);
    }

    static void LogWarning(const char *file, int line, const QString &msg){
        _check_if_initialized();

        GUtil::Logging::GlobalLogger::LogError(
                msg,
                _create_title(file, line),
                GUtil::Logging::GlobalLogger::DebugId);
    }

    static void LogError(const char *file, int line, const QString &msg){
        _check_if_initialized();

        GUtil::Logging::GlobalLogger::LogError(
                msg,
                _create_title(file, line),
                GUtil::Logging::GlobalLogger::DebugId);
    }

    static void LogException(const GUtil::Core::Exception &ex){
        _check_if_initialized();

        GUtil::Logging::GlobalLogger::LogException(ex, GUtil::Logging::GlobalLogger::DebugId);
    }

    static void LogException(const std::exception &ex){
        _check_if_initialized();

        GUtil::Logging::GlobalLogger::LogException(ex, GUtil::Logging::GlobalLogger::DebugId);
    }


private:

    static bool _initialized;

    static QString _create_title(const char *file, int line){
        return QString("%1    Line #%2:").arg(file).arg(line);
    }

    static void _check_if_initialized(){
        if(!_initialized)
            SetupLogFile(DEFAULT_LOG_FILENAME);
    }

};

bool DebugLogger::_initialized = false;

#else

#define dSetupLogFile( fn )
#define dClearLog()

#define dLogVariable( v )

#define dLogMessage( m )
#define dLogWarning( m )
#define dLogError( m )

#define dLogException( e )

#endif

#endif // DEBUGLOGGER_H
