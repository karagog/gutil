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

#include "gthread.h"
using namespace GUtil;

Custom::GThread::GThread(QObject *parent) :
    QThread(parent)
{
    is_cancelled = false;
}

void Custom::GThread::cancel()
{
    is_cancelled = true;
    emit notifyCancelled();
}

void Custom::GThread::uncancel()
{
    is_cancelled = false;
}

bool Custom::GThread::isCancelled()
{
    return is_cancelled;
}

void Custom::GThread::sleep(unsigned long seconds)
{
    QThread::sleep(seconds);
}

void Custom::GThread::msleep(unsigned long mseconds)
{
    QThread::sleep(mseconds);
}

void Custom::GThread::usleep(unsigned long useconds)
{
    QThread::sleep(useconds);
}