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

#ifndef PROCESSSTATUSINDICATOR_H
#define PROCESSSTATUSINDICATOR_H

#include "BusinessObjects/ConfigFile.h"
#include "Utils/usermachinelocks.h"
#include <QObject>

namespace GUtil{ namespace BusinessObjects{



class ProcessStatusIndicator : public QObject
{
    Q_OBJECT
public:

    explicit ProcessStatusIndicator(const QString &process_id, QObject *parent = 0);
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


signals:

    void StatusChanged(int new_status);


private:

    ConfigFile _status_data;
    Utils::UserMachineReadWriteLock _status_lock;


private slots:

    void _status_data_changed();

};


}}

#endif // PROCESSSTATUSINDICATOR_H
