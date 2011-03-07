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

#include "stopwatchengine.h"
#include "gutil_macros.h"
#include <QTimerEvent>
GUTIL_USING_NAMESPACE(Utils);

#define DEFAULT_TIMER_RESOLUTION 100

StopwatchEngine::StopwatchEngine(QObject *parent)
    :QObject(parent),
      m_timer_id(-1),
      m_timer_resolution(DEFAULT_TIMER_RESOLUTION)
{

}

void StopwatchEngine::SetTimerResolution(int milliseconds)
{
    if(IsRunning())
        Stop();

    m_timer_resolution = milliseconds;
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
    if(start)
    {
        if(!IsRunning())
        {
            if(m_startTime.isNull())
                m_startTime = m_time = QDateTime::currentDateTime();

            m_timer_id = startTimer(m_timer_resolution);

            emit NotifyStartedStopped(true);
        }
    }
    else
    {
        if(IsRunning())
        {
            killTimer(m_timer_id);
            m_timer_id = -1;

            m_actual_stopTime = QDateTime::currentDateTime();
            emit NotifyStartedStopped(false);
        }
    }
}

void StopwatchEngine::Reset()
{
    if(IsRunning())
        Stop();
    else
    {
        m_startTime = m_actual_stopTime = m_time = QDateTime();

        emit NotifyTimeChanged();
    }
}

void StopwatchEngine::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId() == m_timer_id)
    {
        m_time = m_time.addMSecs(m_timer_resolution);

        emit NotifyTimeChanged();
    }
}
