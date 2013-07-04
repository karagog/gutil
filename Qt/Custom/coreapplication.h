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
public:

    /** Constructs a CoreApplication with a null application name and version */
    inline CoreApplication(int &argc, char **argv)
        :QCoreApplication(argc, argv)
    {}

    /** Constructs a CoreApplication with the given application version and name */
    inline CoreApplication(int &argc, char **argv,
                              const QString &application_name = QString::null,
                              const QString &application_version = QString::null)
        :QCoreApplication(argc, argv)
    {
        if(!application_name.isEmpty())
            setApplicationName(application_name);
        if(!application_version.isEmpty())
            setApplicationVersion(application_version);
    }

    virtual ~CoreApplication(){}

    virtual bool notify(QObject *o, QEvent *ev){
        bool ret(false);
        try
        {
            ret = QCoreApplication::notify(o, ev);
        }
        catch(Exception<> &ex)
        {
            handle_exception(ex);
        }
        catch(std::exception &ex)
        {
            handle_std_exception(ex);
        }
        return ret;
    }


public slots:

    inline static void Quit(){ Exit(); }

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
