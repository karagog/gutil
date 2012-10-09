/*Copyright 2010-2012 George Karagoulis

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

#ifndef GUTIL_PROCESSSTATUSINDICATOR_H
#define GUTIL_PROCESSSTATUSINDICATOR_H

#include "gutil_persistentdata.h"
#include "gutil_usermachinelocks.h"
#include <QRunnable>
#include <QThreadPool>

class QLocalSocket;
class QLocalServer;

namespace GUtil{ namespace QT{ namespace BusinessObjects{

class ProcessStatusServer;


/** A status indicator that works between processes.

    You can have a main process, and sub-processes exchange information using this
    mechanism.

    \sa ProcessStatusServer
*/
class ProcessStatusIndicator :
        public QObject
{
    Q_OBJECT
public:

    explicit ProcessStatusIndicator(QObject *parent = 0);
    ~ProcessStatusIndicator();
    
    void Initialize();
    void Uninitialize();
    inline bool IsInitialized() const{ return _status_data.IsInitialized(); }

    /** Describes different statuses (implementation specific) */
    enum StatusTypeEnum
    {
        /** Means the process is not running */
        NotRunning = 0,
        /** Means the process is running normally */
        Normal = 1,
        /** Means the process is idling */
        Idle = 2
    };

    /** You can set your own custom statuses, but make sure they're different from
        any in StatusTypeEnum
        \param status_type Any type of status variable.  You implement the statuses.
    */
    void SetStatus(int status_type);

    /** Gets the current process status */
    int GetStatus() const{ return _status_data.Value("status").toInt(); }

    /** Returns true of the process is running */
    bool IsProcessRunning();

    /** Sets the current "isrunning" state.  Other processes can then check if this one
        is actively running
    */
    void SetIsProcessRunning(bool);

    /** Sends a message over a socket to the process controlling the status */
    void SendMessageToControllingProcess(const QString &);

    /** Returns the identity string for the application */
    GUtil::DataObjects::String GetProcessIdentityString() const{
        return _status_data.GetIdentity();
    }


signals:

    /** Notifies when the application status changes */
    void StatusChanged(int new_status);

    /** Notifies when a new message was received by a sub-process */
    void NewMessageReceived(QByteArray);


private:

    ProcessStatusServer *_server;
    PersistentData _status_data;
    Utils::UserMachineReadWriteLock _status_lock;


private slots:

    void _status_data_changed(){
        emit StatusChanged(GetStatus());
    }

};


}}}

#endif // GUTIL_PROCESSSTATUSINDICATOR_H

#endif // GUTIL_NO_NETWORK_FUNCTIONALITY
#endif // GUTIL_NO_GUI_FUNCTIONALITY
