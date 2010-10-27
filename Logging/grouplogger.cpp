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

#include "grouplogger.h"
using namespace GUtil;

Logging::GroupLogger::GroupLogger()
    :AbstractLogger()
{
}

void Logging::GroupLogger::Log(const QString &message, const QString &title, MessageLevelEnum lvl)
{
    // Log the message to all of our loggers
    _logger_set_lock.lockForRead();
    {
        foreach(Logging::AbstractLogger *l, _logger_set)
            l->Log(message, title, lvl);
    }
    _logger_set_lock.unlock();
}

void Logging::GroupLogger::ClearLog()
{
    // Clear all of our loggers
    _logger_set_lock.lockForRead();
    {
        foreach(Logging::AbstractLogger *l, _logger_set)
            l->ClearLog();
    }
    _logger_set_lock.unlock();
}

void Logging::GroupLogger::AddLogger(AbstractLogger *l)
{
    _logger_set_lock.lockForWrite();
    {
        _logger_set.insert(l);
    }
    _logger_set_lock.unlock();
}

void Logging::GroupLogger::RemoveLogger(AbstractLogger *l, bool delete_it)
{
    _logger_set_lock.lockForWrite();
    {
        _remove_logger(l, delete_it);
    }
    _logger_set_lock.unlock();
}

void Logging::GroupLogger::RemoveAllLoggers(bool delete_them)
{
    _logger_set_lock.lockForWrite();
    {
        foreach(AbstractLogger *l, _logger_set)
            _remove_logger(l, delete_them);
    }
    _logger_set_lock.unlock();
}

void Logging::GroupLogger::_remove_logger(AbstractLogger *l, bool del)
{
    if(_logger_set.contains(l))
    {
        _logger_set.remove(l);

        if(del)
            delete l;
    }
}

bool Logging::GroupLogger::ContainsLogger(AbstractLogger *l)
{
    bool ret;

    _logger_set_lock.lockForRead();
    {
        ret = _logger_set.contains(l);
    }
    _logger_set_lock.unlock();

    return ret;
}
