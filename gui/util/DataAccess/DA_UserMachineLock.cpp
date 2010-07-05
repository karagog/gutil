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

#include "DA_UserMachineLock.h"
#include "qtlockedfile.h"
#include <QDir>
using namespace GUtil::QtUtil::DataAccess;

DA_UserMachineLock::DA_UserMachineLock(const QString &id)
{
    QString filename = (QDir::tempPath() + "/%1.userlock").arg(id);
    _lf = new QtLockedFile(filename);

    _locked = false;
    _error = "";
}

DA_UserMachineLock::~DA_UserMachineLock()
{
    Unlock();

    delete _lf;
}

//ApplicationLock::ApplicationLock(const QString &id, const QUuid &reminder)
//{
//    myid = reminder;
//
//    _loaded = init(id);
//}

bool DA_UserMachineLock::Lock()
{
    if(_locked)
        return true;

    if(!_lf->open(QFile::ReadWrite))
    {
        _error = "Could not open file";
        return false;
    }

    if(!_lf->lock(QtLockedFile::WriteLock, false))
    {
        _error = "Could not gain lock on file";
        _lf->close();
        return false;
    }

    return true;
}

void DA_UserMachineLock::Unlock()
{
    if(!_locked)
        return;

    _lf->unlock();
    _lf->close();

    _locked = false;

    QFile::remove(_lf->fileName());
}

QString DA_UserMachineLock::Error() const
{
    return _error;
}

QString DA_UserMachineLock::FileName() const
{
    return _lf->fileName();
}
