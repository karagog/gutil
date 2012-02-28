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

#ifndef GAPPLICATIONBASE_H
#define GAPPLICATIONBASE_H

#include "gutil_macros.h"
#include "gutil_commandlineargs.h"
#include "gutil_extendedexception.h"
#include <QCoreApplication>


#ifdef gApp
    #undef gApp
#endif

/** A reference to the global instance of ApplicationBase.

    This is similar to qApp, but returns ApplicationBase instead.  It is done as a dynamic
    cast, because ApplicationBase is not derived from QApplication, but since Application is
    derived from both we can cross-cast if we do a dynamic cast.  It will return NULL if you
    did not instantiate a Application or GCoreApplication
    \sa qApp
*/
#define gApp   dynamic_cast<GUtil::QT::Custom::ApplicationBase *>(qApp)



NAMESPACE_GUTIL2(QT, Custom);


/** Used as a base class for the common functionality of Application and GCoreApplication.

    Provides a memory management stack, onto which
    you can push any kind of memory, and it will be deleted in FILO order when you call
    Exit() instead of QApplication::exit().
*/
class ApplicationBase
{
public:

    inline ApplicationBase() :m_exiting(0){}

    /** Use this function to quit the application, instead of QApplication::quit()
.
        This will call the virtual function application_exiting()' which you can
        override to put cleanup code in.
    */
    static inline void Exit(int return_code = 0){
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


    /** Get convenient access to the command line arguments. */
    GUtil::Utils::CommandLineArgs Args() const{
        return GUtil::Utils::CommandLineArgs(qApp->argc(), qApp->argv());
    }


    /** Derive from this class to enable yourself to be deleted on the application cleanup stack.
        \sa application_exiting()
    */
    class CleanupObject{
    public:
        /** The virtual destructor allows your derived instance to be deleted by a
            reference to a CleanupObject.
            \sa application_exiting()
        */
        virtual ~CleanupObject(){}
    };


    /** Pushes a cleanup object on the cleanup stack, to be cleaned up at the end of program execution.
        \sa application_exiting()
    */
    void AddCleanupObject(CleanupObject *o){
        if(_cleanup_objects.Contains(o))
            THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                       "Already going to cleanup that object");
        else
            _cleanup_objects << o;
    }


    /** Removes a specified cleanup object from the cleanup stack. */
    inline void RemoveCleanupObject(CleanupObject *o){ _cleanup_objects.RemoveAll(o); }


    /** The stack of objects to be cleaned up at the end of program execution. */
    inline const GUtil::DataObjects::Vector<CleanupObject *> &CleanupObjects() const{
        return _cleanup_objects;
    }


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
    virtual void application_exiting(){
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


    /** You can override this method, which are called in the event of an exception
        during an application event.

        Don't call the base implementation, as it will only throw the exception again.
    */
    virtual void handle_exception(const Exception<> &ex){
        throw ex;
    }


    /** You can override this method, which are called in the event of a std::exception
        during an application event.

        Don't call the base implementation, as it will only throw the exception again.
    */
    virtual void handle_std_exception(const std::exception &ex){
        throw ex;
    }


private:

    GUtil::DataObjects::Vector<CleanupObject *> _cleanup_objects;
    int m_exiting;

};


END_NAMESPACE_GUTIL2;

#endif // GAPPLICATIONBASE_H
