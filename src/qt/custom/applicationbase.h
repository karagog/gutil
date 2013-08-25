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
    did not instantiate a Application or CoreApplication.
    \note You need to compile with RTTI for this to work.
    \sa qApp
*/
#define gApp   dynamic_cast<GUtil::QT::Custom::ApplicationBase *>(qApp)



NAMESPACE_GUTIL2(QT, Custom);


/** Used as a base class for the common functionality of Application and CoreApplication.

    In Linux, it will provide signal handlers which you can override to react to them.

    In Windows, so far this class provides very little extra functionality
*/
class ApplicationBase
{
public:

    /** If this property is set to true, then exceptions handled by this object
        will not be propagated (they will be trapped).  By default this is false.
    */
    PROPERTY(TrapExceptions, bool);

    /** Use this function to quit the application, instead of QApplication::quit()
     *
     *  This calls the virtual function try_exiting() which you can
     *  override to put cleanup code in.  The reason you put cleanup code there instead of the destructor,
     *  is because you need to clean up QObjects while the main event loop is running, and if the
     *  application is being destructed then there is no event loop, so your object cleanup can screw up.
     *
     *  If you want to prevent your application from exiting, then override try_exiting() and throw
     *  an exception (with a helpful message, hopefully).
    */
    static void Exit(int return_code = 0);


    /** Get convenient access to the command line arguments. */
    inline GUtil::Utils::CommandLineArgs Args() const{
        return GUtil::Utils::CommandLineArgs(qApp->argc(), qApp->argv());
    }


    /** This function only works on unix systems, and tells the application to handle the signal number
     *  given.  If you handle a signal, then the protected function handle_os_signal() is called with the signal
     *  number whenever the signal is received.  Some signals cannot be handled (SIG_KILL and SIG_STOP).
     *
     *  \param sig_num The signal identifier, defined in signal.h (see 'man signal')
     *  \returns false if we could not connect to the signal.
     *
     *  \sa IgnoreSignalFromOS(), RestoreDefaultSignalFromOS()
    */
    bool HandleSignalFromOS(int sig_num);

    /** This function only works on unix systems, and tells the application to ignore the signal number
     *  given.  Some signals cannot be ignored (SIG_KILL and SIG_STOP).
     *
     *  \param sig_num The signal identifier, defined in signal.h (see 'man signal')
     *  \returns false if we could not set the disposition of the signal
     *
     *  \sa HandleSignalFromOS(), RestoreDefaultSignalFromOS()
    */
    bool IgnoreSignalFromOS(int sig_num);

    /** This function only works on unix systems, and tells the application to restore the default behavior
     *  for the handling of the given signal.  You would only need to call this to undo a call to either
     *  HandleSignalFromOS() or IgnoreSignalFromOS().
     *
     *  \param sig_num The signal identifier, defined in signal.h (see 'man signal')
     *  \returns false if we could not set the disposition of the signal
     *
     *  \sa HandleSignalFromOS(), IgnoreSignalFromOS()
    */
    bool RestoreDefaultSignalFromOS(int sig_num);


protected:

    /** The constructor is protected to prevent you from instantiating one
        without the rest of the GUtil application.
    */
    ApplicationBase();
    virtual ~ApplicationBase();


    /** Subclasses can override this to implement their own logic for when it's okay to quit.
     *  This is called when you call the Exit() function, and if you throw an exception,
     *  it will effectively prevent the application from exiting.
     *
     *  Do not put cleanup code here.  Put that code in Application::about_to_quit().
     *
     *  \param code The return code we are trying to exit with.  Its meaning is application-specific.
     *
     *  \sa Exit(), Application::about_to_quit(), QCoreApplication::exit()
    */
    virtual void try_exiting(int code);


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


    /** The signal handling function. You can override it to handle os signals (Unix only).
     *
     *  The base implementation does nothing.
     *
     *  See the signal man page for documentation.
    */
    virtual void handle_os_signal(int sig_num);


private:

    /** The actual signal handler function, which delivers the signals to the
        global application object, if it's defined.
    */
    static void _handle_os_signal(int);

};


END_NAMESPACE_GUTIL2;

#endif // GAPPLICATIONBASE_H
