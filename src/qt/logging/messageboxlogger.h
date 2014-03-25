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

#ifndef GUTIL_MESSAGEBOXLOGGER_H
#define GUTIL_MESSAGEBOXLOGGER_H

#include "gutil_abstractlogger.h"

class QWidget;

NAMESPACE_GUTIL1(QT);


/** A logger implementation which displays the message in a modal dialog box.
*/
class MessageBoxLogger :
        public GUtil::AbstractLogger
{
    QWidget *m_parent;
public:

    explicit MessageBoxLogger(QWidget *parent = 0) :AbstractLogger(NULL), m_parent(parent){}

    virtual ~MessageBoxLogger(){}


protected:

    /** Displays a modal dialog box with the title and message, with the
        appropriate severity.
    */
    virtual void log_protected(const LoggingData &);

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_MESSAGEBOXLOGGER_H

#endif // GUI_FUNCTIONALITY
