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

#include "filesystemhelpers.h"
#include "stringhelpers.h"

#include <QDir>
using namespace GUtil;
using namespace GUtil::QtUtil;

bool FilesystemHelpers::rmdir(const QString &path, bool recursive)
{
    QDir tmpdir(path);
    QFile tmpfile(path);
    bool directory;
    bool valid = false;

    if(tmpdir.exists())
    {
        directory = true;
        valid = true;
    }
    else if(tmpfile.exists())
    {
        directory = false;
        valid = true;
    }

    if(!valid)
    {
        qDebug("File/Directory doesn't exist");
        return false;
    }

    QString name(StringHelpers::baseName(
            tmpdir.absolutePath().toStdString()).c_str());

    if(directory)
    {
        QStringList contents = tmpdir.entryList();

        if(!recursive && contents.count() > 0)
        {
            //qDebug("Not deleting directory which has contents when 'recursive' is false");
            return false;
        }

        // Delete the directory's contents before deleting it
        foreach(QString n, contents)
        {
            if(n == "." || n == "..")
                continue;

            if(!rmdir(path + "/" + n))
                return false;
        }

        if(!tmpdir.cdUp())
        {
            qDebug("Can't cd up");
            return false;
        }

        if(!tmpdir.rmdir(name))
        {
            qDebug("Can't delete");
            return false;
        }
    }
    else
    {
        if(!tmpfile.remove())
        {
            qDebug("Can't delete the file: %1s", tmpfile.fileName().toStdString().c_str());
            return false;
        }
    }

    return true;
}

bool FilesystemHelpers::rm(const QString &path)
{
    QDir tmpdir(path);
    QFile tmpfile(path);
    bool directory;
    bool valid = false;

    if(tmpdir.exists())
    {
        directory = true;
        valid = true;
    }
    else if(tmpfile.exists())
    {
        directory = false;
        valid = true;
    }

    if(!valid || directory)
    {
        //qDebug("File doesn't exist, or is a directory");
        return false;
    }

    return rmdir(path);
}
