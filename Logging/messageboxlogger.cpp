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

#include "messageboxlogger.h"
#include "Core/extendedexception.h"
#include <QMessageBox>
using namespace GUtil;

Logging::MessageBoxLogger::MessageBoxLogger(QObject *parent)
    :AbstractLogger(parent)
{
}

Logging::MessageBoxLogger::~MessageBoxLogger()
{

}

void Logging::MessageBoxLogger::Log_protected(const QByteArray &msg, MessageLevelEnum l)
{
    switch(l)
    {
    case Info:
        QMessageBox::information(0, "Info", QString(msg));
        break;
    case Warning:
        QMessageBox::warning(0, "Warning", QString(msg));
        break;
    case Error:
        QMessageBox::critical(0, "ERROR!", QString(msg));
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION2( Core::NotImplementedException, "" );
    }
}

#endif // GUI_FUNCTIONALITY
