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

#include "gapplicationbase.h"
#include <QObject>
GUTIL_USING_NAMESPACE(Custom);

GApplicationCleanerUpper *cleaner(0);
GApplicationBase *gApp(0);

GApplicationBase::GApplicationBase(QCoreApplication *a)
    :m_app(a)
{
    if(gApp)
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "GApplicationBase already initialized!");

    gApp = this;

    cleaner = new GApplicationCleanerUpper(this);
    cleaner->connect(a, SIGNAL(aboutToQuit()), SLOT(cleanup()));
}

GApplicationBase::~GApplicationBase()
{
    if(gApp == this)
    {
        delete cleaner;
        cleaner = 0;

        gApp = 0;
    }
}

void GApplicationBase::AddCleanupObject(GApplicationBase::CleanupObject *o)
{
    if(_cleanup_objects.contains(o))
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "Already going to cleanup that object");
    else
        _cleanup_objects.push(o);
}

void GApplicationBase::RemoveCleanupObject(GApplicationBase::CleanupObject *o)
{
    const int index(_cleanup_objects.indexOf(o));
    if(index != -1)
        _cleanup_objects.remove(index);
}

void GApplicationBase::Cleanup()
{
    cleanup_protected();

    while(_cleanup_objects.count() > 0)
        delete _cleanup_objects.pop();
}
