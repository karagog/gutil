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

#include "filelogger.h"
using namespace GUtil;
using namespace std;

Logging::FileLogger::FileLogger(const QString &filename, QObject *parent)
    :AbstractLogger(parent)
{
    _file_transport.SetFileName(filename);
    _file_transport.SetWriteMode(DataAccess::GFileIODevice::WriteAppend);
}

Logging::FileLogger::~FileLogger()
{
}

void Logging::FileLogger::ClearLog()
{
    try
    {
        _file_transport.TruncateFile();
    }
    catch(Core::Exception<> &)
    {

    }
}

void Logging::FileLogger::Log_protected(const QByteArray &dat, MessageLevelEnum)
{
    _file_transport.Write(dat);
}

QString Logging::FileLogger::FileName() const
{
    return _file_transport.FileName();
}

#endif // GUI_FUNCTIONALITY
