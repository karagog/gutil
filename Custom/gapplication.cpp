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

#include "gapplication.h"
#include "gutil_macros.h"
#include "Core/exception.h"
GUTIL_USING_NAMESPACE(Custom);

GApplication *gApp(0);

GApplication::GApplication(int &argc, char **argv)
    :QApplication(argc, argv)
{
    // Register our cleanup handler
    connect(this, SIGNAL(aboutToQuit()), this, SLOT(cleanup()));

    if(gApp)
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "GApplication already initialized!");

    gApp = this;
}

GApplication::~GApplication()
{
    if(gApp == this)
        gApp = 0;
}

bool GApplication::notify(QObject *o, QEvent *ev)
{
    bool ret(false);

    try
    {
        ret = QApplication::notify(o, ev);
    }
    catch(GUtil::Core::Exception &ex)
    {
        handle_exception(ex);
    }
    catch(std::exception &ex)
    {
        handle_std_exception(ex);
    }

    return ret;
}

void GApplication::AddCleanupObject(GApplication::CleanupObject *o)
{
    if(_cleanup_objects.contains(o))
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "Already going to cleanup that object");
    else
        _cleanup_objects.append(o);
}

void GApplication::RemoveCleanupObject(CleanupObject *o)
{
    if(_cleanup_objects.contains(o))
        _cleanup_objects.removeOne(o);
    else
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "Not tracking that object");
}

void GApplication::cleanup()
{
    foreach(CleanupObject *o, _cleanup_objects)
        delete o;
    _cleanup_objects.clear();
}
