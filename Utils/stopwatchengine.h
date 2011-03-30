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

#ifndef STOPWATCHENGINE_H
#define STOPWATCHENGINE_H

#include <QObject>
#include <QTime>
#include <QReadWriteLock>
#include "gutil_macros.h"

namespace GUtil{ namespace Utils{


class StopwatchEngine :
        public QObject
{
    Q_OBJECT
public:
    explicit StopwatchEngine(QObject *parent = 0);
    virtual ~StopwatchEngine();

    void SetTimerResolution(int milliseconds);
    inline int TimerResolution() const{
        return m_timer_resolution;
    }

    // Controls how many marks to remember.  Default is 1
    PROPERTY( MarkMemoryMaxLength, int );

    inline bool IsRunning() const{
        return m_timer_id != -1;
    }

    inline QDateTime TimeStart() const{
        return m_startTime;
    }
    inline QDateTime TimeStop() const{
        return m_actual_stopTime;
    }

    QDateTime TimeCurrent();
    QDateTime TimeMark(int mark_index = 0);


signals:

    void NotifyStartedStopped(bool started);


public slots:

    void Start();
    void Stop();
    void StartStop(bool start);

    // Records the current datetime and logs it in a list without stopping the timer
    void Mark();

    // Reset stops the timer if its running, or clears the times to null if not.
    void Reset();

    // ResetHot refreshes the start/cur/stop time to the currentDateTime without
    void ResetHot();


protected:

    virtual void timerEvent(QTimerEvent *);


private:

    int m_timer_id;
    int m_timer_resolution;

    QDateTime m_startTime;
    QDateTime m_time;
    QDateTime m_actual_stopTime;
    QList<QDateTime> m_markTimes;

    QReadWriteLock m_lock;

    void _mark_time(const QDateTime &);

};


}}

#endif // STOPWATCHENGINE_H
