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

#include "signalwaitcondition.h"
using namespace GUtil::Custom;

SignalWaitCondition::SignalWaitCondition(QObject *parent)
    :QObject(parent)
{
}

void SignalWaitCondition::WaitForSignal(QMutex *lock,
                                        QObject *obj,
                                        const char *signal,
                                        unsigned long timeout)
{
    connect(obj, signal, this, SLOT(_catch_signal()));

    _wait_condition.wait(lock, timeout);
}

void SignalWaitCondition::WaitForSignal(QReadWriteLock *lock,
                                        QObject *obj,
                                        const char *signal,
                                        unsigned long timeout)
{
    connect(obj, signal, this, SLOT(_catch_signal()));

    _wait_condition.wait(lock, timeout);
}

void SignalWaitCondition::_catch_signal()
{
    disconnect(this, SLOT(_catch_signal()));

    _wait_condition.wakeAll();
}
