/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "stopwatchengine.h"
#include <QTimerEvent>
#include <QTimer>

NAMESPACE_GUTIL2(QT, Utils);


#define DEFAULT_TIMER_RESOLUTION 100

StopwatchEngine::StopwatchEngine(QObject *parent)
    :QObject(parent),
      _p_MarkMemoryMaxLength(1),
      _p_AutoRefreshTime(0),
      m_timer_id(-1),
      m_isRunning(false),
      m_timeAccumulated(0),
      m_timeAccumulatedSinceLastStart(0)
{}

StopwatchEngine::~StopwatchEngine()
{
    Stop();
}

bool StopwatchEngine::IsRunning()
{
    bool ret;
    m_lock.lockForRead();
    ret = m_isRunning;
    m_lock.unlock();
    return ret;
}

bool StopwatchEngine::WasStarted()
{
    bool ret;
    m_lock.lockForRead();
    ret = _wasStarted();
    m_lock.unlock();
    return ret;
}

bool StopwatchEngine::_wasStarted()
{
    _refresh();
    return m_timeAccumulated + m_timeAccumulatedSinceLastStart > 0;
}

void StopwatchEngine::Start()
{
    StartStop(true);
}

void StopwatchEngine::Stop()
{
    StartStop(false);
}

void StopwatchEngine::StartStop(bool start)
{
    m_lock.lockForWrite();
    if(start)
    {
        if(!m_isRunning)
        {
            m_timeAccumulated += m_timeAccumulatedSinceLastStart;
            m_timeAccumulatedSinceLastStart = 0;

            m_time = QDateTime::currentDateTime();
            if(m_startTime.isNull())
                m_startTime = m_time;

            if(GetAutoRefreshTime() > 0)
                m_timer_id = startTimer(GetAutoRefreshTime());

            m_isRunning = true;
        }
    }
    else
    {
        if(m_isRunning)
        {

            if(m_timer_id != -1)
            {
                killTimer(m_timer_id);
                m_timer_id = -1;
            }

            _refresh();
            m_isRunning = false;

            m_stopTime = QDateTime::currentDateTime();
        }
    }
    m_lock.unlock();

    emit NotifyStartedStopped(m_isRunning);
}

void StopwatchEngine::Reset()
{
    m_lock.lockForWrite();
    {
        m_stopTime = QDateTime();
        m_startTime = m_time = QDateTime::currentDateTime();
        m_timeAccumulated = m_timeAccumulatedSinceLastStart = 0;
        m_markTimes.clear();
    }
    m_lock.unlock();
}

void StopwatchEngine::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId() == m_timer_id)
        Refresh();
}

QDateTime StopwatchEngine::TimeCurrent()
{
    QDateTime ret;
    m_lock.lockForRead();
    ret = _timeCurrent();
    m_lock.unlock();
    return ret;
}

QDateTime StopwatchEngine::_timeCurrent()
{
    _refresh();
    return m_startTime.addMSecs(m_timeAccumulated + m_timeAccumulatedSinceLastStart);
}

QDateTime StopwatchEngine::TimeMark(int mark_index)
{
    QDateTime ret;
    m_lock.lockForRead();
    if(mark_index >= 0 && mark_index < m_markTimes.length())
        ret = m_markTimes[mark_index];
    m_lock.unlock();
    return ret;
}

void StopwatchEngine::Mark()
{
    m_lock.lockForWrite();
    _mark_time(_timeCurrent());
    m_lock.unlock();
}

void StopwatchEngine::_mark_time(const QDateTime &dt)
{
    while(m_markTimes.length() >= GetMarkMemoryMaxLength())
        m_markTimes.removeAt(m_markTimes.length() - 1);

    m_markTimes.insert(0, dt);
}

void StopwatchEngine::Refresh()
{
    QDateTime ret;
    m_lock.lockForWrite();
    ret = _timeCurrent();
    m_lock.unlock();

    emit NotifyRefreshed(ret);
}

void StopwatchEngine::_refresh()
{
    if(m_isRunning)
        m_timeAccumulatedSinceLastStart = m_time.msecsTo(QDateTime::currentDateTime());
}

QDateTime StopwatchEngine::TimeStart()
{
    QDateTime ret;
    m_lock.lockForRead();
    {
        // There has only been a valid start time if the timer was started
        if(_wasStarted())
            ret = m_startTime;
    }
    m_lock.unlock();
    return ret;
}

QDateTime StopwatchEngine::TimeStopped()
{
    QDateTime ret;
    m_lock.lockForRead();
    {
        // There is only a valid stop time if the timer was started and is not currently running (was stopped)
        if(!m_isRunning && _wasStarted())
            ret = m_stopTime;
    }
    m_lock.unlock();
    return ret;
}


END_NAMESPACE_GUTIL2;
