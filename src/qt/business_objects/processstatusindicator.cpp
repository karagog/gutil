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
#ifndef GUTIL_NO_NETWORK_FUNCTIONALITY

#include "processstatusindicator.h"
#include "processstatusserver.h"
#include <QLocalSocket>
#include <QLocalServer>
#include <QCoreApplication>

NAMESPACE_GUTIL1(Qt);


#define IDENTITY_FORMAT "PROCESS_STATUS"

ProcessStatusIndicator::ProcessStatusIndicator(QObject *parent)
    :QObject(parent),
      _status_lock(IDENTITY_FORMAT, ""),
      _status_data(IDENTITY_FORMAT)
{
//    connect(&_status_data, SIGNAL(DataChanged()),
//            this, SLOT(_status_data_changed()));
}

ProcessStatusIndicator::~ProcessStatusIndicator()
{
    if(_status_lock.IsLockedOnMachine())
    {
        SetStatus(NotRunning);
        _status_lock.UnlockForMachine();
    }
}

void ProcessStatusIndicator::SetStatus(int newStatus)
{
    if(_status_lock.IsLockedOnMachine())
        _status_data.SetValue("status", newStatus);
    else
        throw Exception<>("You aren't the process that controls the status."
                          "\nYou must first call SetIsRunningProcess(true)");
}

bool ProcessStatusIndicator::IsProcessRunning()
{
    bool ret(false);
    try
    {
        _status_lock.LockForReadOnMachine(false);
    }
    catch(LockException<> &)
    {
        ret = true;
    }

    if(!ret)
        _status_lock.UnlockForMachine();

    return ret;
}

void ProcessStatusIndicator::SetIsProcessRunning(bool is_running)
{
    bool update_status(true);

    if(is_running)
    {
        if(_status_lock.IsLockedOnMachine())
            update_status = false;
        else
        {
            _status_lock.LockForWriteOnMachine(false);
            SetStatus(Normal);
        }
    }
    else
    {
        if(_status_lock.IsLockedOnMachine())
        {
            SetStatus(NotRunning);
            _status_lock.UnlockForMachine();
        }
        else
            update_status = false;
    }

    if(update_status)
    {
        if(is_running)
        {
            _server = new ProcessStatusServer;     // no need for explicit parent; we control memory
            connect(_server, SIGNAL(NewMessage(QByteArray)),
                    this, SIGNAL(NewMessageReceived(QByteArray)));
            _server->listen(GetProcessIdentityString());
        }
        else if(_server)
        {
            _server->close();
            _server.Clear();
        }
    }
}

void ProcessStatusIndicator::SendMessageToControllingProcess(const QString &message)
{
    if(IsProcessRunning())
    {
        QLocalSocket sock;
        sock.connectToServer(GetProcessIdentityString());
        if(sock.waitForConnected())
            sock.write(message.toUtf8());
        else
            throw Exception<>(sock.errorString().toUtf8().constData());
    }
}

void ProcessStatusIndicator::_status_data_changed()
{
    emit StatusChanged(GetStatus());
}


END_NAMESPACE_GUTIL1;

#endif // GUTIL_NO_NETWORK_FUNCTIONALITY
#endif // GUTIL_NO_GUI_FUNCTIONALITY
