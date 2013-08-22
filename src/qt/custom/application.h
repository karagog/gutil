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

#ifndef GUTIL_APPLICATION_H
#define GUTIL_APPLICATION_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include "gutil_applicationbase.h"
#include <QApplication>

namespace GUtil{ namespace QT{ namespace Custom{


/** My derived version of QApplication.

    It has protected constructors, so you have to subclass it.  Then you can
    override the virtual functions to customize exception handling, cleanup logic
    and about logic.
*/
class Application :
        public QApplication,
        public ApplicationBase
{
    Q_OBJECT
protected:

    /** Constructs an instance of Application. */
    Application(int &argc, char **argv);

    /** Constructs an instance of Application.
        \param argc The number of command line parameters, as passed to main()
        \param argv The command line trings, as passed to main()
        \param application_name The name of the application, which determines, among other things,
        the directory paths to the various storage locations.
        \param application_version The current version of the application
        \sa QDesktopServices::storageLocation()
    */
    Application(int &argc, char **argv,
                          const QString &application_name,
                          const QString &application_version = QString::null);


public:

    virtual ~Application();


    /** Overridden from QApplication::notify(), in order to catch exceptions incurred
        during QApplication events, mainly execution of slots.

        You can handle the exception by overriding ApplicationBase::handle_exception()
        \sa ApplicationBase::handle_exception()
    */
    virtual bool notify(QObject *o, QEvent *ev);


public slots:

    /** Shows the application's about window under the given parent widget.
        By default it shows a minimal window with the application name.
        You are supposed to override it with your own informative about window.
    */
    virtual void About(QWidget *parent = 0);

    /** A slot to quit the application and execute custom application cleanup code.
      \sa ApplicationBase::Exit()
    */
    static void Quit();


    /** Show an about window for GUtil.

        Just connect a QPushButton to the gApp instance's slot to show the about.

        C'mon give a guy some credit, I made it super easy for you ;)

        \param dialog_parent The parent widget for the about dialog (can also be left empty)
    */
    static void AboutGUtil(QWidget *dialog_parent = 0);


protected slots:

    /** Subclasses can override this to make their own cleanup code, which will be
        executed when the underlying QApplication emits the signal aboutToQuit().

        You should have the application cleanup memory in the cleanup handler
        rather than the destructor, because the QObject event loops end after QApplication::exec()
        returns, so if you cleanup memory in the destructor you lose the signal/slots functionality,
        which is especially apparent in Linux.  By executing cleanup code in this handler,
        you do so while the application is still fully functional, from a Qt events standpoint.

        If you throw an exception, you can handle it in the handle_exception() function.
    */
    virtual void about_to_quit();


private:

    void _init();

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
