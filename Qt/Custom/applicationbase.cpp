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

#include "applicationbase.h"
#include <signal.h>

NAMESPACE_GUTIL2(QT, Custom);


ApplicationBase::ApplicationBase()
    :m_exiting(0)
{
    if(!_initialize_os_signal_handlers())
        GDEBUG("Some OS signal handlers not registered");
}
    
ApplicationBase::~ApplicationBase(){}

void ApplicationBase::Exit(int return_code)
{
    ApplicationBase *g( gApp );

    // We're allowing exactly one call to this function.  Multiple calls would technically
    //  point to an error in the application code, but the end result is the same (the app exits)
    //  and it will save the programmer some headaches hopefully.  For convenience it is
    //  an int, so you can at least see how many times it was called.
    if(g && g->m_exiting++)
        return;

    // Derived classes will execute their own cleanup code when the application exits
    g->application_exiting();

    QCoreApplication::exit(return_code);
}

void ApplicationBase::AddCleanupObject(CleanupObject *o)
{
    if(_cleanup_objects.Contains(o))
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   "Already going to cleanup that object");
    else
        _cleanup_objects << o;
}

void ApplicationBase::RemoveCleanupObject(CleanupObject *o)
{ 
    _cleanup_objects.RemoveAll(o);
}

void ApplicationBase::application_exiting()
{
    // We put this here, rather than in Exit(), because we force the use to call the base
    //  implementation of this function, which is necessary to conduct the cleanup of the
    //  cleanup objects.  We want every level of subclassing of this class to have the
    //  opportunity to cleanup their own memory, so for correctness of the implementation
    //  everybody must call the base implementation.
    CleanupObject *t;
    while(!_cleanup_objects.IsEmpty()){
        _cleanup_objects >> t;
        delete t;
    }
}

void ApplicationBase::handle_exception(const Exception<> &ex)
{
    throw ex;
}

void ApplicationBase::handle_std_exception(const std::exception &ex)
{
    throw ex;
}

void ApplicationBase::handle_signal_Interrupt(){}
void ApplicationBase::handle_signal_Abort(){}
void ApplicationBase::handle_signal_FloatingPointError(){}
void ApplicationBase::handle_signal_IllegalInstruction(){}
void ApplicationBase::handle_signal_SegmentationFault(){}
void ApplicationBase::handle_signal_Terminate(){}
void ApplicationBase::handle_signal_Hangup(){}

void ApplicationBase::handle_os_signal(int sig_num)
{
    switch(sig_num)
    {
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
#ifdef Q_OS_LINUX
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
    bool ret(true);
    
    if(__os_signal_handlers_initialized)
        ret = false;
    else
    {
        /** \todo Potentially add more signals to handle. */
        if(SIG_ERR == signal(SIGINT, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGABRT, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGFPE, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGILL, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGSEGV, &ApplicationBase::_handle_os_signal)) ret = false;
        if(SIG_ERR == signal(SIGTERM, &ApplicationBase::_handle_os_signal)) ret = false;

        // We can only handle these signals on Posix systems
#ifdef Q_OS_LINUX
        if(SIG_ERR == signal(SIGHUP, &ApplicationBase::_handle_os_signal)) ret = false;
#endif

        __os_signal_handlers_initialized = true;
    }
    
    return ret;
}


END_NAMESPACE_GUTIL2;
