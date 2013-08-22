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
{
    if(!_initialize_os_signal_handlers()){
        GDEBUG("Some OS signal handlers not registered");
    }
}

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

void ApplicationBase::handle_os_signal(int sig_num)
{
    switch(sig_num)
    {


#ifdef Q_OS_LINUX

    case SIGINT:
        this->handle_signal_Interrupt();
        break;
    case SIGABRT:
        this->handle_signal_Abort();
        break;
    case SIGFPE:
        this->handle_signal_FloatingPointError();
        break;
    case SIGILL:
        this->handle_signal_IllegalInstruction();
        break;
    case SIGSEGV:
        this->handle_signal_SegmentationFault();
        break;
    case SIGTERM:
        this->handle_signal_Terminate();
        break;
    case SIGHUP:
        this->handle_signal_Hangup();
        break;

#endif


    default:
        break;
    }
}

void ApplicationBase::_handle_os_signal(int sig_num)
{
    ApplicationBase *appBase = gApp;

    if(appBase)
        appBase->handle_os_signal(sig_num);
}



static bool __os_signal_handlers_initialized(false);

bool ApplicationBase::_initialize_os_signal_handlers()
{
    bool ret(false);

    if(!__os_signal_handlers_initialized)
    {
        ret = true;

#ifdef Q_OS_LINUX
        /** \todo Potentially add more signals to handle. */
        if(SIG_ERR == signal(SIGINT, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGABRT, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGFPE, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGILL, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGSEGV, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGTERM, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGHUP, &ApplicationBase::_handle_os_signal)) ret = false;
#endif

        __os_signal_handlers_initialized = true;
    }

    return ret;
}


// Default handlers do nothing
void ApplicationBase::handle_signal_Interrupt(){}
void ApplicationBase::handle_signal_Abort(){}
void ApplicationBase::handle_signal_FloatingPointError(){}
void ApplicationBase::handle_signal_IllegalInstruction(){}
void ApplicationBase::handle_signal_SegmentationFault(){}
void ApplicationBase::handle_signal_Terminate(){}
void ApplicationBase::handle_signal_Hangup(){}


END_NAMESPACE_GUTIL2;
