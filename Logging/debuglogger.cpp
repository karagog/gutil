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

#include "debuglogger.h"
using namespace GUtil;

bool Logging::DebugLogger::_initialized = false;



void Logging::DebugLogger::SetupLogFile(const QString &fn)
{
    Logging::GlobalLogger::SetupLogger(
            new Logging::FileLogger(fn),
            Logging::GlobalLogger::DebugId);

    _initialized = true;

    ClearLog();
}

void Logging::DebugLogger::ClearLog()
{
    _check_if_initialized();

    Logging::GlobalLogger::ClearLog(Logging::GlobalLogger::DebugId);
}

void Logging::DebugLogger::LogVariable(const char *file, int line,
                                       const QString &var_name, const QVariant &var)
{
    _check_if_initialized();

    QString msg = QString("Variable '%1'    Value: '%2'").arg(var_name).arg(var.toString());

    Logging::GlobalLogger::LogMessage(
            msg,
            _create_title(file, line),
            Logging::GlobalLogger::DebugId);
}

void Logging::DebugLogger::LogMessage(const char *file, int line,
                                      const QString &msg)
{
    _check_if_initialized();

    Logging::GlobalLogger::LogMessage(
            msg,
            _create_title(file, line),
            Logging::GlobalLogger::DebugId);
}

void Logging::DebugLogger::LogWarning(const char *file, int line,
                                      const QString &msg)
{
    _check_if_initialized();

    Logging::GlobalLogger::LogWarning(
            msg,
            _create_title(file, line),
            Logging::GlobalLogger::DebugId);
}

void Logging::DebugLogger::LogError(const char *file, int line,
                                    const QString &msg)
{
    _check_if_initialized();

    Logging::GlobalLogger::LogError(
            msg,
            _create_title(file, line),
            Logging::GlobalLogger::DebugId);
}

void Logging::DebugLogger::LogException(const GUtil::Core::Exception<> &ex)
{
    _check_if_initialized();

    Logging::GlobalLogger::LogException(ex, GUtil::Logging::GlobalLogger::DebugId);
}

void Logging::DebugLogger::LogException(const std::exception &ex)
{
    _check_if_initialized();

    Logging::GlobalLogger::LogException(ex, Logging::GlobalLogger::DebugId);
}

QString Logging::DebugLogger::_create_title(const char *file, int line)
{
    return QString("%1    Line #%2:").arg(file).arg(line);
}

void Logging::DebugLogger::_check_if_initialized()
{
    if(!_initialized)
        SetupLogFile(DEFAULT_LOG_FILENAME);
}

#endif // GUI_FUNCTIONALITY
