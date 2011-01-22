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

#include "processstatusindicator.h"
#include "localsocketclient.h"
#include "localsocketserver.h"
GUTIL_USING_NAMESPACE(BusinessObjects);

#define IDENTITY_FORMAT "%1_PROCESS_STATUS"

ProcessStatusIndicator::ProcessStatusIndicator(const QString &process_id, QObject *parent)
    :QObject(parent),
    _status_data(QString(IDENTITY_FORMAT).arg(process_id)),
    _status_lock(QString(IDENTITY_FORMAT).arg(process_id), ""),
    _server(0)
{
    connect(&_status_data, SIGNAL(NotifyConfigurationUpdate()),
            this, SLOT(_status_data_changed()));
}

ProcessStatusIndicator::~ProcessStatusIndicator()
{
    if(_status_lock.IsLockedOnMachine())
    {
        SetStatus(NotRunning);
        _status_lock.UnlockForMachine();
    }
}

void ProcessStatusIndicator::_status_data_changed()
{
    emit StatusChanged(GetStatus());
}

void ProcessStatusIndicator::SetStatus(int newStatus)
{
    if(_status_lock.IsLockedOnMachine())
        _status_data.SetValue("status", newStatus);
    else
        THROW_NEW_GUTIL_EXCEPTION2(Core::Exception,
                                  "You aren't the process that controls the status."
                                  "\nYou must first call SetIsRunningProcess(true)");
}

int ProcessStatusIndicator::GetStatus()
{
    return _status_data.Value("status").toInt();
}

bool ProcessStatusIndicator::IsProcessRunning()
{
    bool ret(false);
    try
    {
        _status_lock.LockForReadOnMachine(false);
        _status_lock.UnlockForMachine();
    }
    catch(Core::LockException &)
    {
        ret = true;
    }
    return ret;
}

void ProcessStatusIndicator::SetIsProcessRunning(bool is_running)
        throw(Core::Exception)
{
    bool update_status(true);

    if(is_running)
        _status_lock.LockForWriteOnMachine(false);
    else
    {
        if(_status_lock.IsLockedOnMachine())
            _status_lock.UnlockForMachine();
        else
            update_status = false;
    }

    if(update_status)
    {
        SetStatus((int)(is_running ? Normal : NotRunning));
        if(is_running)
        {
            _server = new LocalSocketServer(this);
            _server->ListenForConnections(GetProcessIdentityString());
            connect(_server, SIGNAL(NewMessageArrived()),
                    this, SLOT(_client_message_arrived()));
        }
        else if(_server)
        {
            delete _server;
            _server = 0;
        }
    }
}

void ProcessStatusIndicator::SendMessageToControllingProcess(const QString &message)
{
    if(IsProcessRunning())
    {
        LocalSocketClient sock;
        sock.ConnectToServer(GetProcessIdentityString());
        sock.SendMessage(message.toAscii());
    }
}

QString ProcessStatusIndicator::GetProcessIdentityString() const
{
    QString id, dummy;
    _status_data.GetIdentity(id, dummy);
    return id;
}

void ProcessStatusIndicator::_client_message_arrived()
{
    while(_server->HasMessage())
    {
        emit NewMessageReceived(_server->ReceiveMessage());
    }
}
