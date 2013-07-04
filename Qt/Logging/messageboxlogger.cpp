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
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL2(QT, Logging);


void MessageBoxLogger::log_protected(const String &message,
                                     const String &title,
                                     MessageLevelEnum ml,
                                     time_t)
{
    switch(ml)
    {
    case MessageLevel_Info:
        QMessageBox::information(m_parent, title.ToQString(), message.ToQString());
        break;
    case MessageLevel_Warning:
        QMessageBox::warning(m_parent, title.ToQString(), message.ToQString());
        break;
    case MessageLevel_Error:
        QMessageBox::critical(m_parent, title.ToQString(), message.ToQString());
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION( NotImplementedException );
    }
}


END_NAMESPACE_GUTIL2;

#endif // GUI_FUNCTIONALITY
