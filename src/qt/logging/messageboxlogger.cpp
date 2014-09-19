/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include "messageboxlogger.h"
#include "gutil_extendedexception.h"
#include <QMessageBox>
#include <QDateTime>
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(QT);


void MessageBoxLogger::Log(const LoggingData &d) noexcept
{
    QString title = QString("%1 %2")
            .arg(QDateTime::fromTime_t(d.LogTime).toString())
            .arg(d.Title.ConstData());
    QString msg(d.Message.ConstData());
    switch(d.MessageLevel)
    {
    case LogLevelInfo:
        QMessageBox::information(m_parent, title, msg);
        break;
    case LogLevelWarning:
        QMessageBox::warning(m_parent, title, msg);
        break;
    case LogLevelError:
        QMessageBox::critical(m_parent, title, msg);
        break;
    default:
        break;
    }
}


END_NAMESPACE_GUTIL1;

#endif // GUI_FUNCTIONALITY
