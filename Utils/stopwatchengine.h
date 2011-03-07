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

namespace GUtil{ namespace Utils{


class StopwatchEngine :
        public QObject
{
    Q_OBJECT
public:
    explicit StopwatchEngine(QObject *parent = 0);

    void SetTimerResolution(int milliseconds);
    inline int TimerResolution() const{
        return m_timer_resolution;
    }

    inline bool IsRunning() const{
        return m_timer_id != -1;
    }

    inline QDateTime TimeStart() const{
        return m_startTime;
    }

    // The difference between these two functions is that TimeStop is the actual
    //  Date/Time when the stopwatch was stopped, and TimeEnd represents the effective
    //  time passed, which is TimeStop minus the time in which the stopwatch was
    //  paused.
    inline QDateTime TimeEnd() const{
        return m_time;
    }
    inline QDateTime TimeStop() const{
        return m_actual_stopTime;
    }

signals:

    void NotifyTimeChanged();
    void NotifyStartedStopped(bool started);


public slots:

    void Start();
    void Stop();
    void StartStop(bool start);

    void Reset();


protected:

    virtual void timerEvent(QTimerEvent *);


private:

    int m_timer_id;
    int m_timer_resolution;

    QDateTime m_startTime;
    QDateTime m_time;
    QDateTime m_actual_stopTime;

};


}}

#endif // STOPWATCHENGINE_H
