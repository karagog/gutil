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

#include "cancellablethread.h"
using namespace GUtil::QtUtil::Tools;

CancellableThread::CancellableThread(QObject *parent) :
    QThread(parent)
{
    is_cancelled = false;
}

void CancellableThread::cancel()
{
    is_cancelled = true;
    emit notifyCancelled();
}

void CancellableThread::uncancel()
{
    is_cancelled = false;
}

bool CancellableThread::isCancelled()
{
    return is_cancelled;
}
