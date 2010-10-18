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
#include "filelogger.h"
#include "abstractlogger.h"
#include <QString>
#include <QMap>
#include <QReadWriteLock>
using namespace GUtil;

BusinessObjects::GlobalLogger *global_logger = 0;
QMap<int, BusinessObjects::AbstractLogger *> global_logger_list;
QReadWriteLock global_logger_list_lock;

BusinessObjects::GlobalLogger::GlobalLogger()
    :QObject()
{
}

int BusinessObjects::GlobalLogger::SetupLogger(BusinessObjects::AbstractLogger *logger, int logger_id)
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

int BusinessObjects::GlobalLogger::SetupDefaultLogger(const QString &filename)
{
    return SetupLogger(new BusinessObjects::FileLogger(filename), 0);
}

void BusinessObjects::GlobalLogger::TakeDownLogger(int logger_id)
{
    global_logger_list_lock.lockForWrite();
    _takedown_logger(logger_id);
    global_logger_list_lock.unlock();
}

void BusinessObjects::GlobalLogger::TakeDownDefaultLogger()
{
    TakeDownLogger(0);
}

BusinessObjects::GlobalLogger *BusinessObjects::GlobalLogger::Instance()
{
    if(global_logger == 0)
        global_logger = new GlobalLogger();

    return global_logger;
}

void BusinessObjects::GlobalLogger::ClearLog(int logger_id)
{
    global_logger_list_lock.lockForRead();
    if(global_logger_list.contains(logger_id))
        global_logger_list.value(logger_id)->ClearLog();
    global_logger_list_lock.unlock();
}

void BusinessObjects::GlobalLogger::_takedown_logger(int logger_id)
{
    if(global_logger_list.contains(logger_id))
    {
        delete global_logger_list.value(logger_id);
        global_logger_list.remove(logger_id);
    }
}

void BusinessObjects::GlobalLogger::LogMessage(const QString &msg, const QString &title, int logger_id)
{
    Log(msg, title, logger_id, (int)BusinessObjects::AbstractLogger::Info);
}

void BusinessObjects::GlobalLogger::LogWarning(const QString &msg, const QString &title, int logger_id)
{
    Log(msg, title, logger_id, (int)BusinessObjects::AbstractLogger::Warning);
}

void BusinessObjects::GlobalLogger::LogError(const QString &msg, const QString &title, int logger_id)
{
    Log(msg, title, logger_id, (int)BusinessObjects::AbstractLogger::Error);
}

void BusinessObjects::GlobalLogger::LogException(const Core::Exception &ex, int logger_id)
{
    global_logger_list_lock.lockForRead();

    if(global_logger_list.contains(logger_id))
        global_logger_list.value(logger_id)->LogException(ex);

    global_logger_list_lock.unlock();
}

void BusinessObjects::GlobalLogger::LogException(const std::exception &ex, int logger_id)
{
    global_logger_list_lock.lockForRead();

    if(global_logger_list.contains(logger_id))
        global_logger_list.value(logger_id)->LogException(ex);

    global_logger_list_lock.unlock();
}

void BusinessObjects::GlobalLogger::Log(const QString &msg,
                                        const QString &title,
                                        int logger_id,
                                        int message_type_enum)
{
    global_logger_list_lock.lockForRead();

    if(global_logger_list.contains(logger_id))
        global_logger_list.value(logger_id)->Log(
                msg, title,
                (BusinessObjects::AbstractLogger::MessageLevelEnum)message_type_enum);

    global_logger_list_lock.unlock();
}
