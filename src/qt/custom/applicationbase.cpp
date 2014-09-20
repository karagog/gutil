/*Copyright 2010-2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "applicationbase.h"
#include "gutil_globallogger.h"

#ifdef Q_OS_LINUX
#include <signal.h>
#endif

NAMESPACE_GUTIL1(QT);


ApplicationBase::ApplicationBase()
    :_p_TrapExceptions(false)
{}

ApplicationBase::~ApplicationBase(){}

void ApplicationBase::Exit(int return_code)
{
    ApplicationBase *a = gApp;
    if(a)
        a->try_exiting(return_code);
    else{
        GDEBUG("WARNING:  Exit() called in non-GUtil Application!"
               "  This is not inherently dangerous, but it is probably not intended.");
    }

    QCoreApplication::exit(return_code);
}

void ApplicationBase::try_exiting(int){}

void ApplicationBase::handle_exception(std::exception &ex)
{
    GlobalLogger().LogException(ex);
    if(!GetTrapExceptions())
        throw;  // We're inside of a catch handler, so this rethrows
}


void ApplicationBase::handle_os_signal(int){}

void ApplicationBase::_handle_os_signal(int sig_num)
{
    ApplicationBase *appBase = gApp;

    if(appBase)
        appBase->handle_os_signal(sig_num);
}



// Define OS-specific function implementations:

#ifdef Q_OS_LINUX

bool ApplicationBase::HandleSignalFromOS(int sig_num)
{
    bool ret = false;
    if(SIG_ERR != signal(sig_num, &ApplicationBase::_handle_os_signal))
        ret = true;
    return ret;
}

bool ApplicationBase::IgnoreSignalFromOS(int sig_num)
{
    bool ret = false;
    if(SIG_ERR != signal(sig_num, SIG_IGN))
        ret = true;
    return ret;
}

bool ApplicationBase::RestoreDefaultSignalFromOS(int sig_num)
{
    bool ret = false;
    if(SIG_ERR != signal(sig_num, SIG_DFL))
        ret = true;
    return ret;
}

#else  // No OS signals functionality

bool ApplicationBase::HandleSignalFromOS(int)
{
    return false;
}

bool ApplicationBase::IgnoreSignalFromOS(int)
{
    return false;
}

bool ApplicationBase::RestoreDefaultSignalFromOS(int)
{
    return false;
}

#endif


END_NAMESPACE_GUTIL1;
