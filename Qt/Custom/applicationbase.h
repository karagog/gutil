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

#ifndef GAPPLICATIONBASE_H
#define GAPPLICATIONBASE_H

#include "gutil_macros.h"
#include "gutil_commandlineargs.h"
#include "gutil_extendedexception.h"
#include "gutil_abstractlogger.h"
#include <QCoreApplication>


#ifdef gApp
    #undef gApp
#endif

/** A reference to the global instance of ApplicationBase.

    This is similar to qApp, but returns ApplicationBase instead.  It is done as a dynamic
    cast, because ApplicationBase is not derived from QApplication, but since Application is
    derived from both we can cross-cast if we do a dynamic cast.  It will return NULL if you
    did not instantiate a Application or CoreApplication
    \sa qApp
*/
#define gApp   dynamic_cast<GUtil::QT::Custom::ApplicationBase *>(qApp)



NAMESPACE_GUTIL2(QT, Custom);


/** Used as a base class for the common functionality of Application and CoreApplication.

    Provides a memory management stack, onto which
    you can push any kind of memory, and it will be deleted in FILO order when you call
    Exit() instead of QApplication::exit().
*/
class ApplicationBase
{
    ::GUtil::Logging::AbstractLogger *m_logger;
public:

    /** If this property is set to true, then exceptions handled by this object
        will not be propagated (they will be trapped).  By default this is false.
    */
    PROPERTY(TrapExceptions, bool);

    /** Use this function to quit the application, instead of QApplication::quit()

        This will call the virtual function 'application_exiting()' which you can
        override to put cleanup code in.
    */
    static void Exit(int return_code = 0);


    /** Get convenient access to the command line arguments. */
    inline GUtil::Utils::CommandLineArgs Args() const{
        return GUtil::Utils::CommandLineArgs(qApp->argc(), qApp->argv());
    }


protected:

    /** The constructor is protected to prevent you from instantiating one
        without the rest of the GUtil application.
        \param An optional logger that the application will use to log messages.
        The application will not own the logger's memory.
    */
    explicit ApplicationBase(::GUtil::Logging::AbstractLogger *logger = 0);
    virtual ~ApplicationBase();


    /** Subclasses can override this to make their own cleanup code, which will be
        executed on a call to the public static function Exit().

        You should have the application cleanup memory in the cleanup handler
        rather than the destructor, because the QObject event loops end after QApplication::exec()
        returns, so if you cleanup memory in the destructor you lose the signal/slots functionality,
        which is especially apparent in Linux.  By executing cleanup code in this handler,
        you do so while the application is still fully functional, from a Qt events standpoint.

        \note You must call the base implementation, which does the actual cleanup of the CleanupObjects.
        \sa Exit() QCoreApplication::exit()
    */
    virtual void application_exiting();


    /** You can override this method, which are called in the event of an exception
        during an application event.

        In the base implementation, the exception is automatically logged to the
        logger given in the constructor.  The exception will either be rethrown or
        trapped, depending on the 'TrapExceptions' property.
    */
    virtual void handle_exception(const Exception<> &);


    /** You can override this method, which are called in the event of a std::exception
        during an application event.
        
        In the base implementation, the exception is automatically logged to the
        logger given in the constructor.  The exception will either be rethrown or
        trapped, depending on the 'TrapExceptions' property.
    */
    virtual void handle_std_exception(const std::exception &);



    /** \name Operating System Signal Handlers

        You can override these functions to handle specific operating system signals.
        
        \note If the operating system does not use signals, then these handlers
        may never be called.  This is mostly for Unix systems.

        \{
    */


    /** The master signal routing function.  You could override it if you need to,
        but generally you should override the specific signal handler you are interested in.
    */
    virtual void handle_os_signal(int sig_num);

    
    /** Handles SIGINT (defined in <signals.h>).
        This handles the Ctrl-C event from the user.
    */
    virtual void handle_signal_Interrupt();
    
    /** Handles SIGABRT (defined in <signals.h>).
        This means the process called abort() somewhere (debug assertion?)
    */
    virtual void handle_signal_Abort();
    
    /** Handles SIGFPE (defined in <signals.h>).
        There was an error with an arithmetic calculation, like an overflow.
    */
    virtual void handle_signal_FloatingPointError();
    
    /** Handles SIGILL (defined in <signals.h>).
        This means that the program tried to execute an illegal instruction.
    */
    virtual void handle_signal_IllegalInstruction();
    
    /** Handles SIGSEGV (defined in <signals.h>).
        This means there was a memory segmentation fault.
    */
    virtual void handle_signal_SegmentationFault();
    
    /** Handles SIGTERM (defined in <signals.h>).
        This handles the termination signal.
    */
    virtual void handle_signal_Terminate();
    
    /** Handles SIGHUP (defined in <signals.h>).
        This signal means that the calling process is terminating.
        \note This is only available on POSIX systems.
    */
    virtual void handle_signal_Hangup();


    /** \} */
    

private:

    int m_exiting;


    /** Registers the signal handler function for various OS signals. */
    static bool _initialize_os_signal_handlers();

    /** The actual signal handler function, which delivers the signals to the
        global application object, if it's defined.
    */
    static void _handle_os_signal(int);

};


END_NAMESPACE_GUTIL2;

#endif // GAPPLICATIONBASE_H
