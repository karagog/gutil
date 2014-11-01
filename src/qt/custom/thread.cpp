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

#include "thread.h"

NAMESPACE_GUTIL1(Qt);


Thread::Thread(QObject *parent)
    :QThread(parent),
      is_cancelled(false)
{}

void Thread::cancel()
{
    thread_lock.lockForWrite();
    {
        is_cancelled = true;
    }
    thread_lock.unlock();

    thread_wait_condition.wakeAll();
    emit notifyCancelled();
}

bool Thread::isCancelled()
{
    bool ret;
    thread_lock.lockForRead();
    {
        ret = is_cancelled;
    }
    thread_lock.unlock();
    return ret;
}

void Thread::sleep(GUINT32 seconds)
{
    QThread::sleep(seconds);
}

void Thread::msleep(GUINT32 mseconds)
{
    QThread::msleep(mseconds);
}

void Thread::usleep(GUINT32 useconds)
{
    QThread::usleep(useconds);
}


END_NAMESPACE_GUTIL1;
