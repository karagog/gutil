/*Copyright 2010-2015 George Karagoulis

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
#include <QMessageBox>
#include <QDateTime>
#include <QMetaType>
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(Qt);


MessageBoxLogger::MessageBoxLogger(QWidget *parent)
    :QObject((QObject *)parent),
      m_parentWidget(parent)
{
    qRegisterMetaType<LoggingData>("LoggingData");
}

void MessageBoxLogger::Log(const LoggingData &d) noexcept
{
    // Invoke this as a queued signal so the logging happens in our event loop
    QMetaObject::invokeMethod(this, "_log", ::Qt::AutoConnection,
                              Q_ARG(LoggingData, d));
}

void MessageBoxLogger::_log(const LoggingData &d)
{
    QString title = QString("%1 %2")
            .arg(QDateTime::fromTime_t(d.LogTime).toString())
            .arg(d.Title.ConstData());
    QString msg(d.Message.ConstData());
    switch(d.MessageLevel)
    {
    case LogLevelInfo:
        QMessageBox::information(m_parentWidget, title, msg);
        break;
    case LogLevelWarning:
        QMessageBox::warning(m_parentWidget, title, msg);
        break;
    case LogLevelError:
        QMessageBox::critical(m_parentWidget, title, msg);
        break;
    default:
        break;
    }
}


END_NAMESPACE_GUTIL1;

#endif // GUI_FUNCTIONALITY
