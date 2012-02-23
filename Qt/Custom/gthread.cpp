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

#include "gthread.h"

NAMESPACE_GUTIL2(QT, Custom);


GThread::GThread(QObject *parent) :
    QThread(parent),
    is_cancelled(false)
{}

void GThread::cancel()
{
    thread_lock.lockForWrite();
    {
        is_cancelled = true;
    }
    thread_lock.unlock();

    thread_wait_condition.wakeAll();
    emit notifyCancelled();
}

bool GThread::isCancelled()
{
    bool ret;
    thread_lock.lockForRead();
    {
        ret = is_cancelled;
    }
    thread_lock.unlock();
    return ret;
}

void GThread::sleep(GUINT32 seconds)
{
    QThread::sleep(seconds);
}

void GThread::msleep(GUINT32 mseconds)
{
    QThread::sleep(mseconds);
}

void GThread::usleep(GUINT32 useconds)
{
    QThread::sleep(useconds);
}


END_NAMESPACE_GUTIL2;
