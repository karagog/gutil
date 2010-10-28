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

void Logging::GroupLogger::Log(const QString &message, const QString &title, MessageLevelEnum lvl)
{
    if(lvl < MessageLevel())
        return;

    // Log the message to all of our loggers
    _logger_list_lock.lockForRead();
    {
        foreach(AbstractLogger *l, _logger_list)
            l->Log(message, title, lvl);
    }
    _logger_list_lock.unlock();
}

void Logging::GroupLogger::ClearLog()
{
    // Clear all of our loggers
    _logger_list_lock.lockForRead();
    {
        foreach(AbstractLogger *l, _logger_list)
            l->ClearLog();
    }
    _logger_list_lock.unlock();
}

void Logging::GroupLogger::AddLogger(AbstractLogger *l)
{
    _logger_list_lock.lockForWrite();
    {
        _logger_list.append(l);
    }
    _logger_list_lock.unlock();
}

void Logging::GroupLogger::RemoveLogger(AbstractLogger *l)
{
    _logger_list_lock.lockForWrite();
    {
        _logger_list.removeAll(l);
    }
    _logger_list_lock.unlock();
}
