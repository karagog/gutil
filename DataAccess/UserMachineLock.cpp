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

#include "UserMachineLock.h"
#include "qtlockedfile.h"
#include <QDir>
#include <QUuid>
using namespace GUtil;

DataAccess::UserMachineLock::UserMachineLock(const QString &id, const QString &modifier,
                                             Utils::AbstractLogger *logger, QObject *parent)
    :ConfigFile(id, modifier, logger, parent)
{
    SetReadOnly(true);
    _lf_lock = new QtLockedFile(QString("%1.%2")
                          .arg(ConfigFile::fileName())
                          .arg("lockfile"));
}

DataAccess::UserMachineLock::~UserMachineLock()
{
    unlock();

    delete _lf_lock;
}

void DataAccess::UserMachineLock::lock()
{
    bool ret;
    bool lock_failed = false;
    QString errmsg;

    if((ret = isLocked()));

    else if(!(ret = _lf_lock->open(QFile::ReadWrite)))
        errmsg = _lf_lock->errorString();

    else if(!(ret = _lf_lock->lock(QtLockedFile::WriteLock, false)))
    {
        _lf_lock->close();
        lock_failed = true;
    }

    // If we don't have a lock, then we're stuck in readonly mode
    SetReadOnly(!ret);

    if(!ret && !lock_failed)
        throw Core::Exception(errmsg.toStdString());
}

void DataAccess::UserMachineLock::unlock()
{
    if(!isLocked())
        return;

    // If we don't have a lock, then we're stuck in readonly mode
    SetReadOnly(true);

    _lf_lock->unlock();
    _lf_lock->close();

    QFile::remove(_lf_lock->fileName());
}

bool DataAccess::UserMachineLock::isLocked() const
{
    return _lf_lock->isLocked();
}

QString DataAccess::UserMachineLock::fileName() const
{
    return _lf_lock->fileName();
}

std::string DataAccess::UserMachineLock::ReadonlyMessageIdentifier() const
{
    return "DataAccess::UserMachineLock";
}
