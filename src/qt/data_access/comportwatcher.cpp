/*Copyright 2014-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "comportwatcher.h"
#include <QSerialPortInfo>
#include <QTimerEvent>
#include <gutil/macros.h>

NAMESPACE_GUTIL1(Qt);


QStringList COMPortWatcher::AvailablePorts()
{
    QStringList ret;
    for(const QSerialPortInfo &i : QSerialPortInfo::availablePorts())
        ret << i.portName();
    return ret;
}


COMPortWatcher::COMPortWatcher(QObject *parent)
    :QObject(parent),
      m_tid(-1)
{}

void COMPortWatcher::StartWatching(int interval_ms)
{
    Q_ASSERT(-1 == m_tid);
    m_tid = startTimer(interval_ms);
}

void COMPortWatcher::StopWatching()
{
    Q_ASSERT(-1 != m_tid);
    killTimer(m_tid);
    m_tid = -1;
    m_availablePorts.clear();
}

void COMPortWatcher::timerEvent(QTimerEvent *ev)
{
    Q_UNUSED(ev);

    bool updated = false;
    QStringList ap = AvailablePorts();
    m_lock.lock();
    if(ap != m_availablePorts){
        m_availablePorts = ap;
        updated = true;
    }
    m_lock.unlock();

    if(updated)
        emit NotifyAvailableCOMPortsChanged();
}

QStringList COMPortWatcher::GetAvailablePorts()
{
    QStringList ret;
    m_lock.lock();
    ret = m_availablePorts;
    m_lock.unlock();
    return ret;
}


END_NAMESPACE_GUTIL1;
