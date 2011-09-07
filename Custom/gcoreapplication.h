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

#ifndef GCOREAPPLICATION_H
#define GCOREAPPLICATION_H

#include <QCoreApplication>
#include "gapplicationbase.h"

namespace GUtil{ namespace Custom {


class GCoreApplication :
        public QCoreApplication,
        public GApplicationBase
{
    Q_OBJECT
public:

    explicit GCoreApplication(int &argc, char **argv,
                              const QString &application_name = QString::null,
                              const QString &application_version = QString::null);
    virtual ~GCoreApplication(){}

    virtual bool notify(QObject *, QEvent *);


public slots:

    static void Quit();

};


}}


#ifdef gApp
    #undef gApp
#endif

/** A reference to the global instance of GCoreApplication.

    This is similar to qApp, but returns GCoreApplication instead.  It is done as a dynamic
    cast, so in case you didn't actually instantiate a GCoreApplication it will return NULL.
    \sa qApp
*/
#define gApp   dynamic_cast<GUtil::Custom::GCoreApplication *>(qApp)


#endif // GCOREAPPLICATION_H
