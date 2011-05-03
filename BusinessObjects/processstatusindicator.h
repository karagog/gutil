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

#if defined(GUI_FUNCTIONALITY) && defined(NETWORK_FUNCTIONALITY)

#ifndef PROCESSSTATUSINDICATOR_H
#define PROCESSSTATUSINDICATOR_H

#include "BusinessObjects/ConfigFile.h"
#include "Utils/usermachinelocks.h"
#include <QLocalServer>
#include <QRunnable>
#include <QThreadPool>

class QLocalSocket;

namespace GUtil{ namespace BusinessObjects{


class ProcessServer;


class ProcessStatusIndicator :
        public QObject
{
    Q_OBJECT
public:

    explicit ProcessStatusIndicator(const QString &directory,
                                    const QString &process_id,
                                    QObject *parent = 0);
    ~ProcessStatusIndicator();

    enum StatusTypeEnum
    {
        NotRunning = 0,
        Normal = 1,
        Idle = 2
    };

    // You set your own custom statuses, but make sure they're different from
    //  any in StatusTypeEnum
    void SetStatus(int status_type);
    int GetStatus();

    bool IsProcessRunning();
    void SetIsProcessRunning(bool)
            throw(Core::Exception);

    void SendMessageToControllingProcess(const QString &);

    QString GetProcessIdentityString() const;


signals:

    void StatusChanged(int new_status);
    void NewMessageReceived(QByteArray);


private:

    ProcessServer *_server;
    ConfigFile _status_data;
    Utils::UserMachineReadWriteLock _status_lock;


private slots:

    void _status_data_changed();

};





// This implementation is supposed to be private.  Only use
//  the class above.
class ProcessThread :
        public QRunnable
{
public:
    inline ProcessThread(ProcessServer *server, quintptr socket_descriptor)
        :_sd(socket_descriptor),
          _server(server)
    {}
private:
    void run();
    quintptr _sd;
    ProcessServer *_server;
};


class ProcessServer :
        public QLocalServer
{
    Q_OBJECT
    friend class ProcessThread;
public:
    explicit ProcessServer(QObject *p = 0)
        :QLocalServer(p)
    {}
    inline ~ProcessServer(){
        thread_pool.waitForDone();
    }

signals:
    void NewMessage(QByteArray);

protected:
    void notify_new_message(const QByteArray &);

private:
    void incomingConnection(quintptr socketDescriptor);
    QThreadPool thread_pool;
};


}}

#endif // PROCESSSTATUSINDICATOR_H

#endif // GUI_FUNCTIONALITY && NETWORK_FUNCTIONALITY
