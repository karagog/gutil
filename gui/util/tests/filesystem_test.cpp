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

#include "filesystem_test.h"
#include "filesystemhelpers.h"

#include <QDir>
#include <QTest>

using namespace GUtil::QtUtil;

filesystem_test::filesystem_test(QObject *parent) :
    QObject(parent)
{
}


void filesystem_test::rmdir_test()
{
    QDir tmpdir = QDir::temp();
    //qDebug((QString("Using temp directory: ") + tmpdir.absolutePath()).toStdString().c_str());

    QString dirstr = "GTestLib_tempdir";

    if(!tmpdir.exists(dirstr))
        QVERIFY(tmpdir.mkdir(dirstr));

    // Empty directory
    QString tmpdir_str = tmpdir.absolutePath() + "/" + dirstr;
    QVERIFY(FilesystemHelpers::rmdir(tmpdir_str));

    // Directory with files in it
    QVERIFY(tmpdir.mkdir(dirstr));
    QFile tmpfile(tmpdir_str + "/GTestLib_tempfile");
    tmpfile.open(QFile::WriteOnly);
    tmpfile.write("Hi");
    tmpfile.close();
    QVERIFY(!FilesystemHelpers::rmdir(tmpdir_str, false));
    QVERIFY(FilesystemHelpers::rmdir(tmpdir_str));
}

void filesystem_test::rm_test()
{
    // Delete a normal file
    QFile tmpfile(QDir::tempPath() + "/" + "GTestLib_tempfile");
    QString probe = QDir::tempPath();
    if(!tmpfile.exists())
    {
        QVERIFY(tmpfile.open(QFile::WriteOnly));
        tmpfile.close();
    }

    QVERIFY(FilesystemHelpers::rm(tmpfile.fileName()));

    // Delete a directory (should fail, because rm is for files only)
    QDir tmpdir = QDir::temp();
    QString dirstr = "GTestLib_tempdir";

    if(!tmpdir.exists(dirstr))
        QVERIFY(tmpdir.mkdir(dirstr));

    dirstr = tmpdir.absolutePath() + "/" + dirstr;
    QVERIFY(!FilesystemHelpers::rm(dirstr));

    // actually delete it when we're done
    FilesystemHelpers::rmdir(dirstr);
}
