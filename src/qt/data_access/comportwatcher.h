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

#ifndef COMPORTWATCHER_H
#define COMPORTWATCHER_H

#ifndef GUTIL_NO_SERIAL_FUNCTIONALITY

#include <QObject>
#include <QStringList>
#include <QMutex>

namespace GUtil{ namespace Qt{


class COMPortWatcher : public QObject
{
    Q_OBJECT
public:
    explicit COMPortWatcher(QObject *parent = 0);

    /** Tells the class to start or stop watching. */
    void StartWatching(int interval_ms);

    void StopWatching();

    /** Return a list of available serial port names. */
    static QStringList AvailablePorts();
    QStringList GetAvailablePorts();

protected:

    virtual void timerEvent(QTimerEvent *);

signals:
    void NotifyAvailableCOMPortsChanged();


private:
    QMutex m_lock;
    QStringList m_availablePorts;

    int m_tid;
};


}}

#endif // GUTIL_NO_SERIAL_FUNCTIONALITY

#endif // COMPORTWATCHER_H
