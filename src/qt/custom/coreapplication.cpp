/*Copyright 2013-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "coreapplication.h"
#include <memory>
using namespace std;

NAMESPACE_GUTIL1(Qt);


CoreApplication::CoreApplication(int &argc, char **argv)
    :QCoreApplication(argc, argv),
      ApplicationBase(argc, argv)
{
    _init();
}

CoreApplication::CoreApplication(int &argc, char **argv, const QString &application_name, const QString &application_version)
    :QCoreApplication(argc, argv),
      ApplicationBase(argc, argv)
{
    if(!application_name.isEmpty())
        setApplicationName(application_name);
    if(!application_version.isEmpty())
        setApplicationVersion(application_version);

    _init();
}

void CoreApplication::_init()
{
    connect(this, SIGNAL(aboutToQuit()), this, SLOT(about_to_quit()));
}

CoreApplication::~CoreApplication()
{}

void CoreApplication::Quit()
{
    Exit();
}

bool CoreApplication::notify(QObject *o, QEvent *ev)
{
    bool ret(false);
    try{
        ret = QCoreApplication::notify(o, ev);
    }
    catch(exception &ex){
        handle_exception(ex);
    }
    catch(const shared_ptr<GUtil::Exception<>> &ex){
        handle_exception(*ex);
    }
    return ret;
}

void CoreApplication::about_to_quit(){}


END_NAMESPACE_GUTIL1;
