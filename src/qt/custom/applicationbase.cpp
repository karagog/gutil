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

#include "applicationbase.h"

#ifdef Q_OS_LINUX
#include <signal.h>
#endif

USING_NAMESPACE_GUTIL1(Logging);

NAMESPACE_GUTIL2(QT, Custom);


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

void ApplicationBase::handle_exception(const Exception<> &ex)
{
    if(!GetTrapExceptions())
        throw ex;
}

void ApplicationBase::handle_std_exception(const std::exception &ex)
{
    if(!GetTrapExceptions())
        throw ex;
}

void ApplicationBase::handle_os_signal(int){}

void ApplicationBase::_handle_os_signal(int sig_num)
{
    ApplicationBase *appBase = gApp;

    if(appBase)
        appBase->handle_os_signal(sig_num);
}



bool ApplicationBase::HandleSignalFromOS(int sig_num)
{
    bool ret = false;

#ifdef Q_OS_LINUX
    if(SIG_ERR != signal(sig_num, &ApplicationBase::_handle_os_signal))
        ret = true;
#endif

    return ret;
}

bool ApplicationBase::IgnoreSignalFromOS(int sig_num)
{
    bool ret = false;

#ifdef Q_OS_LINUX
    if(SIG_ERR != signal(sig_num, SIG_IGN))
        ret = true;
#endif

    return ret;
}

bool ApplicationBase::RestoreDefaultSignalFromOS(int sig_num)
{
    bool ret = false;

#ifdef Q_OS_LINUX
    if(SIG_ERR != signal(sig_num, SIG_DFL))
        ret = true;
#endif

    return ret;
}


END_NAMESPACE_GUTIL2;
