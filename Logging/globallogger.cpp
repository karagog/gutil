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
#include "Logging/abstractlogger.h"
#include <QReadWriteLock>
#include <QVariant>
#include <QApplication>
using namespace GUtil;

// Global variables
QMap<int, Logging::AbstractLogger *> Logging::GlobalLogger::_logger_list;
QReadWriteLock Logging::GlobalLogger::_logger_list_lock;

int Logging::GlobalLogger::_default_logger_id = 0;



int Logging::GlobalLogger::SetupLogger(Logging::AbstractLogger *l, int logger_id)
{
    int ret;
    _logger_list_lock.lockForWrite();
    try
    {
        _translate_logger_id(logger_id, true);
        ret = _setup_logger(l, logger_id);
    }
    catch(...)
    {
        _logger_list_lock.unlock();
        throw;
    }
    _logger_list_lock.unlock();
    return ret;
}

int Logging::GlobalLogger::SetupDefaultLogger(Logging::AbstractLogger *l)
{
    return SetupLogger(l, DefaultId);
}

int Logging::GlobalLogger::_setup_logger(Logging::AbstractLogger *logger, int logger_id)
{
    _takedown_logger(logger_id);
    _logger_list.insert(logger_id, logger);
    return logger_id;
}

void Logging::GlobalLogger::TakeDownLogger(int logger_id)
{
    _logger_list_lock.lockForWrite();
    try
    {
        _translate_logger_id(logger_id, false);
        _takedown_logger(logger_id);
    }
    catch(...)
    {
        _logger_list_lock.unlock();
        throw;
    }
    _logger_list_lock.unlock();
}

void Logging::GlobalLogger::_takedown_logger(int logger_id)
{
    if(_logger_list.contains(logger_id))
    {
        _logger_list.value(logger_id)->deleteLater();
        _logger_list.remove(logger_id);
    }
}

void Logging::GlobalLogger::_translate_logger_id(int &id, bool allow_new_id)
        throw(Core::ArgumentException)
{
    if(id == DebugId);
    else if(id == DefaultId)
        id = _default_logger_id;
    else if(id == NewId)
    {
        if(!allow_new_id)
            THROW_NEW_GUTIL_EXCEPTION2( Core::ArgumentException, "Can't create new ID here" );

        // Auto-assign a logger id
        id = 1;
        while(_logger_list.contains(id))
            id++;
    }
    else if(id < 0)
    {
        Core::ArgumentException ex("Logger ID not recognized");
        ex.SetData("id", QVariant(id).toString().toStdString());
        THROW_GUTIL_EXCEPTION( ex );
    }
}

void Logging::GlobalLogger::ClearLog(int logger_id)
{
    if(logger_id == NewId)
        return;

    _logger_list_lock.lockForRead();
    try
    {
        _translate_logger_id(logger_id, false);

        if(_logger_list.contains(logger_id))
            _logger_list.value(logger_id)->ClearLog();
    }
    catch(...)
    {
        _logger_list_lock.unlock();
        throw;
    }
    _logger_list_lock.unlock();
}

void Logging::GlobalLogger::SetDefaultLoggerID(int new_id)
{
    _logger_list_lock.lockForWrite();
    try
    {
        _translate_logger_id(new_id, false);

        if(_logger_list.contains(new_id))
            _default_logger_id = new_id;
    }
    catch(...)
    {
        _logger_list_lock.unlock();
        throw;
    }
    _logger_list_lock.unlock();
}

int Logging::GlobalLogger::GetDefaultLoggerID()
{
    int ret;
    _logger_list_lock.lockForRead();
    {
        ret = _default_logger_id;
    }
    _logger_list_lock.unlock();
    return ret;
}

void Logging::GlobalLogger::LogMessage(const QString &msg, const QString &title, int logger_id)
{
    Log(msg, title, logger_id, (int)Logging::AbstractLogger::Info);
}

void Logging::GlobalLogger::LogWarning(const QString &msg, const QString &title, int logger_id)
{
    Log(msg, title, logger_id, (int)Logging::AbstractLogger::Warning);
}

void Logging::GlobalLogger::LogError(const QString &msg, const QString &title, int logger_id)
{
    Log(msg, title, logger_id, (int)Logging::AbstractLogger::Error);
}

void Logging::GlobalLogger::LogException(const GUtil::Core::Exception &ex, int logger_id)
{
    _logger_list_lock.lockForRead();
    try
    {
        _translate_logger_id(logger_id, false);
        if(_logger_list.contains(logger_id))
            _logger_list.value(logger_id)->LogException(ex);
    }
    catch(...)
    {
        _logger_list_lock.unlock();
        throw;
    }
    _logger_list_lock.unlock();
}

void Logging::GlobalLogger::LogException(const std::exception &ex, int logger_id)
{
    _logger_list_lock.lockForRead();
    try
    {
        _translate_logger_id(logger_id, false);
        if(_logger_list.contains(logger_id))
            _logger_list.value(logger_id)->LogException(ex);
    }
    catch(...)
    {
        _logger_list_lock.unlock();
        throw;
    }
    _logger_list_lock.unlock();
}

void Logging::GlobalLogger::Log(const QString &msg,
                                const QString &title,
                                int logger_id,
                                int message_level)
{
    _logger_list_lock.lockForRead();
    try
    {
        _translate_logger_id(logger_id, false);

        if(_logger_list.contains(logger_id))
        {
            _logger_list.value(logger_id)->Log(
                    msg, title,
                    (Logging::AbstractLogger::MessageLevelEnum)message_level);
        }
    }
    catch(...)
    {
        _logger_list_lock.unlock();
        throw;
    }
    _logger_list_lock.unlock();
}
