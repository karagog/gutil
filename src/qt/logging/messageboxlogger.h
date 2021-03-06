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

#ifndef GUTIL_MESSAGEBOXLOGGER_H
#define GUTIL_MESSAGEBOXLOGGER_H

#include <gutil/ilog.h>
#include <QObject>
class QWidget;

namespace GUtil{ namespace Qt{


/** A logger implementation which displays the message in a modal dialog box. */
class MessageBoxLogger : public QObject, public GUtil::ILog
{
    Q_OBJECT
    QWidget *m_parentWidget;
public:

    explicit MessageBoxLogger(QWidget *parent = 0);
    virtual ~MessageBoxLogger() {}

    void SetParentWidget(QWidget *parent){ m_parentWidget = parent; }
    QWidget *GetParentWidget() const{ return m_parentWidget; }

    /** Displays a modal dialog box with the title and message, with the
        appropriate severity.
    */
    virtual void Log(const LoggingData &) noexcept;


private slots:

    void _log(const LoggingData &);

};


}}

#endif // GUTIL_MESSAGEBOXLOGGER_H

#endif // GUI_FUNCTIONALITY
