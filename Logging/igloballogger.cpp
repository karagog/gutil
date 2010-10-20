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

#include "igloballogger.h"
#include "filelogger.h"
#include "consolelogger.h"
#include "abstractlogger.h"
#include <QString>
#include <QMap>
#include <QReadWriteLock>
#include <QApplication>
using namespace GUtil;

QMap<int, Logging::AbstractLogger *> global_logger_list;
QReadWriteLock global_logger_list_lock;

Logging::IGlobalLogger::IGlobalLogger()
{
    _default_logger_id = 0;
}

Logging::IGlobalLogger::~IGlobalLogger()
{

}

int Logging::IGlobalLogger::SetupLogger(Logging::AbstractLogger *logger)
{
    return _setup_logger(logger, -1);
}

int Logging::IGlobalLogger::_setup_logger(Logging::AbstractLogger *logger, int logger_id)
{
    global_logger_list_lock.lockForWrite();;
    {
        if(logger_id == -1)
        {
            // Auto-assign a logger id
            logger_id = 1;
            while(global_logger_list.contains(logger_id))
                logger_id++;
        }
        else
            _takedown_logger(logger_id);

        global_logger_list.insert(logger_id, logger);
    }
    global_logger_list_lock.unlock();

    return logger_id;
}

int Logging::IGlobalLogger::SetupFileLogger(const QString &filename)
{
    return _setup_logger(new Logging::FileLogger(filename, qApp), GetDefaultLogger());
}

int Logging::IGlobalLogger::SetupConsoleLogger()
{
    return _setup_logger(new Logging::ConsoleLogger(qApp), GetDefaultLogger());
}

void Logging::IGlobalLogger::TakeDownLogger(int logger_id)
{
    _translate_logger_id(logger_id);

    global_logger_list_lock.lockForWrite();
    _takedown_logger(logger_id);
    global_logger_list_lock.unlock();
}

void Logging::IGlobalLogger::SetDefaultLogger(int new_id)
{
    global_logger_list_lock.lockForRead();
    if(global_logger_list.contains(new_id))
        _default_logger_id = new_id;
    global_logger_list_lock.unlock();
}

int Logging::IGlobalLogger::GetDefaultLogger()
{
    return _default_logger_id;
}

void Logging::IGlobalLogger::ClearLog(int logger_id)
{
    _translate_logger_id(logger_id);

    global_logger_list_lock.lockForRead();
    if(global_logger_list.contains(logger_id))
        global_logger_list.value(logger_id)->ClearLog();
    global_logger_list_lock.unlock();
}

void Logging::IGlobalLogger::_takedown_logger(int logger_id)
{
    if(global_logger_list.contains(logger_id))
    {
        global_logger_list.value(logger_id)->deleteLater();
        global_logger_list.remove(logger_id);
    }
}

void Logging::IGlobalLogger::_translate_logger_id(int &id)
{
    if(id == -1)
        id = GetDefaultLogger();
}

void Logging::IGlobalLogger::LogMessage(const QString &msg,
                       const QString &title,
                       int logger_id)
{
    Log(msg, title, logger_id, (int)Logging::AbstractLogger::Info);
}

void Logging::IGlobalLogger::LogWarning(const QString &msg, const QString &title, int logger_id)
{
    Log(msg, title, logger_id, (int)Logging::AbstractLogger::Info);
}

void Logging::IGlobalLogger::LogError(const QString &msg, const QString &title, int logger_id)
{
    Log(msg, title, logger_id, (int)Logging::AbstractLogger::Error);
}

void Logging::IGlobalLogger::LogException(const Core::Exception &ex, int logger_id)
{
    _translate_logger_id(logger_id);

    global_logger_list_lock.lockForRead();

    if(global_logger_list.contains(logger_id))
        global_logger_list.value(logger_id)->LogException(ex);

    global_logger_list_lock.unlock();
}

void Logging::IGlobalLogger::LogException(const std::exception &ex, int logger_id)
{
    _translate_logger_id(logger_id);

    global_logger_list_lock.lockForRead();

    if(global_logger_list.contains(logger_id))
        global_logger_list.value(logger_id)->LogException(ex);

    global_logger_list_lock.unlock();
}

void Logging::IGlobalLogger::Log(const QString &msg,
                                        const QString &title,
                                        int logger_id,
                                        int message_type_enum)
{
    _translate_logger_id(logger_id);

    global_logger_list_lock.lockForRead();

    if(global_logger_list.contains(logger_id))
        global_logger_list.value(logger_id)->Log(
                msg, title,
                (Logging::AbstractLogger::MessageLevelEnum)message_type_enum);

    global_logger_list_lock.unlock();
}
