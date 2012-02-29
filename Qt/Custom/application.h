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

#ifndef GUTIL_APPLICATION_H
#define GUTIL_APPLICATION_H

#ifdef GUI_FUNCTIONALITY

#include "gutil_applicationbase.h"
#include "gutil_processstatusindicator.h"
#include "gutil_aboutgutil.h"
#include <QApplication>

namespace GUtil{ namespace QT{ namespace Custom{


/** My derived version of QApplication.

    It derives from QApplication, so you can use it instead of one when
    running your Qt application.  It provides several benefits, including
    ipc functionality right out of the box.
*/
class Application :
        public QApplication,
        public ApplicationBase
{
    Q_OBJECT
public:

    /** Constructs an instance of Application. */
    inline Application(int &argc, char **argv) :QApplication(argc, argv) {}

    /** Constructs an instance of Application.
        \param application_name The name of the application, which determines, among other things,
        the directory paths to the various storage locations.
        \param application_version The current version of the application
        \sa QDesktopServices::storageLocation()
    */
    inline Application(int &argc, char **argv,
                          const QString &application_name,
                          const QString &application_version = QString::null)
        :QApplication(argc, argv)
    {
        if(!application_name.isEmpty())
            setApplicationName(application_name);
        if(!application_version.isEmpty())
            setApplicationVersion(application_version);

        #ifdef NETWORK_FUNCTIONALITY
        // Initialize this after setting the application name and version
        m_psi = new BusinessObjects::ProcessStatusIndicator;
        #endif
    }


    #ifdef NETWORK_FUNCTIONALITY

    /** To facilitate Inter-process communication between application instances.

        This is useful when the application is already running when the user tries
        to run another instance.  You can have the newly-started process hail the
        the already-running process and send control signals to it.

        \note You must define NETWORK_FUNCTIONALITY to use this feature, which creates
        a link dependency on QtNetwork
    */
    inline GUtil::QT::BusinessObjects::ProcessStatusIndicator &GlobalApplicationStatusIndicator(){
        return *m_psi;
    }

    #endif


    /** Overridden from QApplication::notify(), in order to catch exceptions incurred
        during QApplication events, mainly execution of slots.

        You can handle the exception by overriding ApplicationBase::handle_exception()
        \sa ApplicationBase::handle_exception()
    */
    virtual bool notify(QObject *o, QEvent *ev){
        bool ret(false);
        try{
            ret = QApplication::notify(o, ev);
        }
        catch(Exception<> &ex){
            handle_exception(ex);
        }
        catch(std::exception &ex){
            handle_std_exception(ex);
        }
        return ret;
    }


public slots:

    /** A slot to quit the application and execute custom application cleanup code.
      \sa ApplicationBase::Exit()
    */
    inline static void Quit(){ Exit(); }


    /** Show an about window for GUtil.

        Just connect a QPushButton to the gApp instance's slot to show the about.

        C'mon give a guy some credit, I made it super easy for you ;)
    */
    static void AboutGUtil(){ GUtil::QT::UI::AboutGUtil().exec(); }


protected:

    #ifdef NETWORK_FUNCTIONALITY
    /** We have to delete the status indicator in the cleanup handler. */
    virtual void application_exiting(){
        delete m_psi;
        m_psi = 0;
        ApplicationBase::application_exiting();
    }
    #endif // NETWORK_FUNCTIONALITY


private:

    #ifdef NETWORK_FUNCTIONALITY
    GUtil::QT::BusinessObjects::ProcessStatusIndicator *m_psi;
    #endif // NETWORK_FUNCTIONALITY

};


}}}


#ifdef gApp
    #undef gApp
#endif

/** A reference to the global instance of Application.

    This is similar to qApp, but returns Application instead.  It is done as a dynamic
    cast, so in case you didn't actually instantiate a Application it will return NULL.
    \sa qApp
*/
#define gApp   dynamic_cast<GUtil::QT::Custom::Application *>(qApp)


#endif // GUI_FUNCTIONALITY

#endif // GUTIL_APPLICATION_H
