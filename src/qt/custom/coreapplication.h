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

#ifndef GUTIL_COREAPPLICATION_H
#define GUTIL_COREAPPLICATION_H

#include <QCoreApplication>
#include "applicationbase.h"

namespace GUtil{ namespace QT{ namespace Custom {


/** A customized QCoreApplication, which implements the Application functionality */
class CoreApplication :
        public QCoreApplication,
        public ApplicationBase
{
    Q_OBJECT
protected:

    /** Constructs a CoreApplication with a null application name and version */
    CoreApplication(int &argc, char **argv);

    /** Constructs a CoreApplication with the given application version and name */
    CoreApplication(int &argc, char **argv,
                    const QString &application_name = QString::null,
                    const QString &application_version = QString::null);


public:

    /** Overridden from QCoreApplication::notify(), in order to catch exceptions incurred
        during events, mainly execution of slots.

        You can handle the exception by overriding ApplicationBase::handle_exception()
        \sa ApplicationBase::handle_exception()
    */
    virtual bool notify(QObject *o, QEvent *ev);

    virtual ~CoreApplication();


public slots:

    /** A slot to quit the application and execute custom application cleanup code.
      \sa ApplicationBase::Exit()
    */
    static void Quit();


protected slots:

    /** Subclasses can override this to make their own cleanup code, which will be
        executed when the underlying QCoreApplication emits the signal aboutToQuit().

        You should have the application cleanup memory in the cleanup handler
        rather than the destructor, because the QObject event loops end after QApplication::exec()
        returns, so if you cleanup memory in the destructor you lose the signal/slots functionality,
        which is especially apparent in Linux.  By executing cleanup code in this handler,
        you do so while the application is still fully functional, from a Qt events standpoint.

        If you throw an exception, you can handle it in the handle_exception() function.
    */
    void about_to_quit();


private:

    void _init();

};


}}}


#ifdef gApp
    #undef gApp
#endif

/** A reference to the global instance of CoreApplication.

    This is similar to qApp, but returns CoreApplication instead.  It is done as a dynamic
    cast, so in case you didn't actually instantiate a CoreApplication it will return NULL.
    \sa qApp
*/
#define gApp   dynamic_cast<GUtil::Custom::CoreApplication *>(qApp)


#endif // GUTIL_COREAPPLICATION_H
