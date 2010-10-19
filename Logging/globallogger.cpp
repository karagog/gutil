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

#include "globallogger.h"
#include "igloballogger.h"
using namespace GUtil;

Logging::GlobalLogger this_instance;
Logging::IGlobalLogger global_logger;

Logging::GlobalLogger::GlobalLogger(QObject *parent) :
    QObject(parent)
{
}

Logging::GlobalLogger *Logging::GlobalLogger::Instance()
{
    return &this_instance;
}

int Logging::GlobalLogger::SetupLogger(Logging::AbstractLogger *l)
{
    return global_logger.SetupLogger(l);
}

int Logging::GlobalLogger::SetupFileLogger(const QString &filename)
{
    return global_logger.SetupFileLogger(filename);
}

int Logging::GlobalLogger::SetupConsoleLogger()
{
    return global_logger.SetupConsoleLogger();
}

void Logging::GlobalLogger::TakedownLogger(int logger_id)
{
    if(logger_id == -1)
        global_logger.TakeDownLogger();
    else
        global_logger.TakeDownLogger(logger_id);
}

void Logging::GlobalLogger::ClearLog(int logger_id)
{
    if(logger_id == -1)
        global_logger.ClearLog();
    else
        global_logger.ClearLog(logger_id);
}

void Logging::GlobalLogger::SetDefaultLogger(int id)
{
    global_logger.SetDefaultLogger(id);
}

int Logging::GlobalLogger::GetDefaultLogger()
{
    return global_logger.GetDefaultLogger();
}

void Logging::GlobalLogger::LogMessage(const QString &msg, const QString &title, int logger_id)
{
    if(logger_id == -1)
        global_logger.LogMessage(msg, title);
    else
        global_logger.LogMessage(msg, title, logger_id);
}

void Logging::GlobalLogger::LogWarning(const QString &msg, const QString &title, int logger_id)
{
    if(logger_id == -1)
        global_logger.LogWarning(msg, title);
    else
        global_logger.LogWarning(msg, title, logger_id);
}

void Logging::GlobalLogger::LogError(const QString &msg, const QString &title, int logger_id)
{
    if(logger_id == -1)
        global_logger.LogError(msg, title);
    else
        global_logger.LogError(msg, title, logger_id);
}

void Logging::GlobalLogger::LogException(const GUtil::Core::Exception &ex, int logger_id)
{
    if(logger_id == -1)
        global_logger.LogException(ex);
    else
        global_logger.LogException(ex, logger_id);
}

void Logging::GlobalLogger::LogException(const std::exception &ex, int logger_id)
{
    if(logger_id == -1)
        global_logger.LogException(ex);
    else
        global_logger.LogException(ex, logger_id);
}

void Logging::GlobalLogger::Log(const QString &msg,
                                        const QString &title,
                                        int logger_id,
                                        int message_level)
{
    global_logger.Log(msg, title, logger_id, message_level);
}
