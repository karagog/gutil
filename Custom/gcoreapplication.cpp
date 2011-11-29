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

#include "gcoreapplication.h"
#include "gutil_macros.h"
GUTIL_USING_NAMESPACE(Custom);

GCoreApplication::GCoreApplication(int &argc, char **argv,
                                   const QString &application_name,
                                   const QString &application_version)
    :QCoreApplication(argc, argv)
{
    if(!application_name.isNull())
        setApplicationName(application_name);
    if(!application_version.isNull())
        setApplicationVersion(application_version);
}

bool GCoreApplication::notify(QObject *o, QEvent *ev)
{
    bool ret(false);

    try
    {
        ret = QCoreApplication::notify(o, ev);
    }
    catch(GUtil::Core::Exception<> &ex)
    {
        handle_exception(ex);
    }
    catch(std::exception &ex)
    {
        handle_std_exception(ex);
    }

    return ret;
}

void GCoreApplication::Quit()
{
    Exit();
}
