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

#include "filelogger.h"
#include "pubsubsystem.h"
#include <fstream>
using namespace GQtUtil::Utils;
using namespace std;

FileLogger::FileLogger(const QString &filename, PubSubSystem *pss, QObject *parent) :
    StreamLogger(new fstream(), pss, parent)
{
    _filename = filename;
}

bool FileLogger::PreLogMessage()
{
    FileStream()->open(_filename.toStdString().c_str(), ios_base::app);

    return FileStream()->is_open();
}

void FileLogger::PostLogMessage()
{
    FileStream()->close();
}

void FileLogger::Clear()
{
    FileStream()->open(_filename.toStdString().c_str(), ios_base::trunc);
    FileStream()->close();
}

fstream *FileLogger::FileStream()
{
    return (fstream *)Stream();
}
