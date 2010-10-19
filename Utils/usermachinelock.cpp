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
#include "ThirdParty/QtLockedFile/qtlockedfile.h"
#include <QDesktopServices>
using namespace GUtil;

Utils::UserMachineLock::UserMachineLock(const QString &id, const QString &modifier)
{
    _usermachinelockfile = new QtLockedFile();

    SetUserMachineLockIdentifier(id, modifier);
}

Utils::UserMachineLock::~UserMachineLock()
{
    UnlockForUserOnMachine();

    delete _usermachinelockfile;
}

void Utils::UserMachineLock::SetUserMachineLockIdentifier(
        const QString &identifier,
        const QString &modifier)
{
    if(identifier.length() == 0)
        return;

    SetUserMachineLockFileName(QString("%1/%2%3.MACHINE_LOCK")
                               .arg(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
                               .arg(identifier)
                               .arg(modifier.length() != 0 ?
                                    QString(".%1").arg(modifier) :
                                    QString::null));
}

void Utils::UserMachineLock::SetUserMachineLockFileName(const QString &fn)
{
    UnlockForUserOnMachine();

    _usermachinelockfile->setFileName(fn);
}

bool Utils::UserMachineLock::LockForUserOnMachine(bool block)
            throw(Core::Exception)
{
    if(FileNameForUserMachineLock() == QString::null)
        throw Core::Exception("The machine-lock file has not been set.  You must "
                              "provide an identifier and optional modifier to use "
                              "this function");

    if(IsLockedForUserOnMachine())
    {
        Core::Exception ex("Already locked!");
        ex.SetData("Filename", FileNameForUserMachineLock().toStdString());
        throw ex;
    }

    else if(!_usermachinelockfile->open(QFile::ReadWrite))
    {
        Core::Exception ex("Couldn't open lockfile!");
        ex.SetData("Filename", FileNameForUserMachineLock().toStdString());
        ex.SetData("Error", _usermachinelockfile->errorString().toStdString());
        throw ex;
    }

    else if(!_usermachinelockfile->lock(QtLockedFile::WriteLock, block))
        _usermachinelockfile->close();

    return IsLockedForUserOnMachine();
}

void Utils::UserMachineLock::UnlockForUserOnMachine()
{
    if(!IsLockedForUserOnMachine())
        return;

    _usermachinelockfile->unlock();
    _usermachinelockfile->close();
}

bool Utils::UserMachineLock::IsLockedForUserOnMachine() const
{
    return _usermachinelockfile->isLocked();
}

QString Utils::UserMachineLock::FileNameForUserMachineLock() const
{
    return _usermachinelockfile->fileName();
}
