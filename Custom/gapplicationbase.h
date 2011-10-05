/*Copyright 2011 George Karagoulis

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

#include "gutil.h"
#include "Core/Utils/commandlineargs.h"
#include "Core/exception.h"
#include <QCoreApplication>
#include <QStack>

namespace GUtil{ namespace Custom{


/** Used as a base class for the common functionality of GApplication and GCoreApplication.

    Provides a memory management stack, onto which
    you can push any kind of memory, and it will be deleted in FILO order when you call
    Exit() instead of QApplication::exit().
*/
class GApplicationBase
{
public:

    GApplicationBase();

    /** Use this function to quit the application, instead of QApplication::quit()
.
        This will call the virtual function application_exiting()' which you can
        override to put cleanup code in.
    */
    static void Exit(int return_code = 0);


    /** Get convenient access to the command line arguments. */
    Core::Utils::CommandLineArgs Args() const;


    /** Derive from this class to enable yourself to be deleted on the application cleanup stack.
        \sa application_exiting()
    */
    class CleanupObject{
    public:
        /** The virtual destructor allows your derived instance to be deleted by a
            reference to a CleanupObject.
            \sa application_exiting()
        */
        virtual ~CleanupObject();
    };


    /** Pushes a cleanup object on the cleanup stack, to be cleaned up at the end of program execution.
        \sa application_exiting()
    */
    void AddCleanupObject(CleanupObject *o);


    /** Removes a specified cleanup object from the cleanup stack. */
    void RemoveCleanupObject(CleanupObject *o);


    /** The stack of objects to be cleaned up at the end of program execution. */
    inline QStack<CleanupObject *> CleanupObjects() const{ return _cleanup_objects; }


protected:

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

        Don't call the base implementation, as it will only throw the exception again.
    */
    virtual void handle_exception(const Core::Exception &);


    /** You can override this method, which are called in the event of a std::exception
        during an application event.

        Don't call the base implementation, as it will only throw the exception again.
    */
    virtual void handle_std_exception(const std::exception &);


private:

    QStack<CleanupObject *> _cleanup_objects;
    int m_exiting;

};


}}


#ifdef gApp
    #undef gApp
#endif

/** A reference to the global instance of GApplicationBase.

    This is similar to qApp, but returns GApplicationBase instead.  It is done as a dynamic
    cast, because GApplicationBase is not derived from QApplication, but since GApplication is
    derived from both we can cross-cast if we do a dynamic cast.  It will return NULL if you
    did not instantiate a GApplication or GCoreApplication
    \sa qApp
*/
#define gApp   dynamic_cast<GUtil::Custom::GApplicationBase *>(qApp)


#endif // GAPPLICATIONBASE_H
