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

#ifndef STOPWATCHENGINE_H
#define STOPWATCHENGINE_H

#include <QObject>
#include <QTime>
#include <QReadWriteLock>
#include "gutil_macros.h"

namespace GUtil{ namespace QT{ namespace Utils{


// This class behaves like a stopwatch.  It is thread safe.
class StopwatchEngine :
        public QObject
{
    Q_OBJECT
public:
    explicit StopwatchEngine(QObject *parent = 0);
    virtual ~StopwatchEngine();

    // Controls how many marks to remember.  Default is 1
    PROPERTY( MarkMemoryMaxLength, int );

    // If you want the stopwatch to automatically refresh, set this time value
    //  (in milliseconds) to how often you want it to refresh.  Default is 0
    // This property will take effect the next time you call Start()
    PROPERTY( AutoRefreshTime, int );

    bool IsRunning();
    bool WasStarted();

    QDateTime TimeStart();
    QDateTime TimeStopped();

    // Returns the time index's current value
    QDateTime TimeCurrent();

    QDateTime TimeMark(int mark_index = 0);


signals:

    void NotifyStartedStopped(bool started);

    // Called every time the internal state was updated
    void NotifyRefreshed(QDateTime current = QDateTime());


public slots:

    // Refresh the internal state of the stopwatch; i.e. Update the current date
    void Refresh();

    void Start();
    void Stop();
    void StartStop(bool start);

    // Records the current datetime and logs it in a list without stopping the timer
    void Mark();

    // Reset stops the timer if its running, or clears the times to null if not.
    void Reset();


protected:

    virtual void timerEvent(QTimerEvent *);


private:

    int m_timer_id;

    bool m_isRunning;
    quint64 m_timeAccumulated;
    quint64 m_timeAccumulatedSinceLastStart;

    QDateTime m_startTime;
    QDateTime m_stopTime;
    QDateTime m_time;
    QList<QDateTime> m_markTimes;



    QReadWriteLock m_lock;

    void _mark_time(const QDateTime &);
    void _refresh();
    QDateTime _timeCurrent();
    bool _wasStarted();

};


}}}

#endif // STOPWATCHENGINE_H
