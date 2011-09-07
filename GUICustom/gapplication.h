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

#ifndef GAPPLICATION_H
#define GAPPLICATION_H

#include <QApplication>
#include "Custom/gapplicationbase.h"
#include "BusinessObjects/processstatusindicator.h"

namespace GUtil{ namespace GUICustom{


/** My derived version of QApplication.

    It derives from QApplication, so you can use it instead of one when
    running your Qt application.  It provides several benefits, including
    ipc functionality right out of the box.
*/
class GApplication :
        public QApplication,
        public Custom::GApplicationBase
{
    Q_OBJECT
public:

    /** Constructs an instance of GApplication.
        \param application_name The name of the application, which determines, among other things,
        the directory paths to the various storage locations.
        \param application_version The current version of the application
        \sa QDesktopServices::storageLocation()
    */
    explicit GApplication(int &argc, char **argv,
                          const QString &application_name = QString::null,
                          const QString &application_version = QString::null);


    #ifdef NETWORK_FUNCTIONALITY

    /** To facilitate Inter-process communication between application instances.

        This is useful when the application is already running when the user tries
        to run another instance.  You can have the newly-started process hail the
        the already-running process and send control signals to it.

        \note You must define NETWORK_FUNCTIONALITY to use this feature, which creates
        a link dependency on QtNetwork
    */
    inline GUtil::BusinessObjects::ProcessStatusIndicator &GlobalApplicationStatusIndicator(){
        return *m_psi;
    }

    #endif


    /** Overridden from QApplication::notify(), in order to catch exceptions incurred
        during QApplication events, mainly execution of slots.

        You can handle the exception by overriding GApplicationBase::handle_exception()
        \sa GApplicationBase::handle_exception()
    */
    virtual bool notify(QObject *, QEvent *);


public slots:

    /** A slot to quit the application and execute custom application cleanup code.
      \sa GApplicationBase::Exit()
    */
    static void Quit();


    /** Show an about window for GUtil.

        Just connect a QPushButton to the gApp instance's slot to show the about.

        C'mon give a guy some credit, I made it super easy for you ;)
    */
    static void AboutGUtil();


protected:

    #ifdef NETWORK_FUNCTIONALITY
    /** We have to delete the status indicator in the cleanup handler. */
    virtual void application_exiting();
    #endif // NETWORK_FUNCTIONALITY


private:

    #ifdef NETWORK_FUNCTIONALITY
    GUtil::BusinessObjects::ProcessStatusIndicator *m_psi;
    #endif // NETWORK_FUNCTIONALITY

};


}}


#ifdef gApp
    #undef gApp
#endif

/** A reference to the global instance of GApplication.

    This is similar to qApp, but returns GApplication instead.  It is done as a dynamic
    cast, so in case you didn't actually instantiate a GApplication it will return NULL.
    \sa qApp
*/
#define gApp   dynamic_cast<GUtil::Custom::GApplication *>(qApp)


#endif // GAPPLICATION_H
