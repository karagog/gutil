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

// This class is just a facade; it is here because GApplicationBase cannot be
//  a QObject itself, because it is expected that it will be derived by other
//  derivitives of QObject
class CleanerUpper :
        public QObject
{
    Q_OBJECT
public:
    explicit CleanerUpper(GApplicationBase *ga)
        :m_gapp(ga) {}
public slots:
    void cleanup(){
        m_gapp->Cleanup();
    }
private:
    GApplicationBase *m_gapp;
};

CleanerUpper *cleaner(0);

GApplicationBase::GApplicationBase(QCoreApplication *a)
    :m_app(a)
{
    cleaner = new CleanerUpper(this);
    cleaner->connect(a, SIGNAL(aboutToQuit()), SLOT(cleanup()));
}

GApplicationBase::~GApplicationBase()
{
    delete cleaner;
    cleaner = 0;
}

void GApplicationBase::AddCleanupObject(GApplicationBase::CleanupObject *o)
{
    if(_cleanup_objects.contains(o))
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "Already going to cleanup that object");
    else
        _cleanup_objects.append(o);
}

void GApplicationBase::RemoveCleanupObject(CleanupObject *o)
{
    if(_cleanup_objects.contains(o))
        _cleanup_objects.removeOne(o);
    else
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "Not tracking that object");
}

void GApplicationBase::Cleanup()
{
    cleanup_protected();

    foreach(CleanupObject *o, _cleanup_objects)
        delete o;
    _cleanup_objects.clear();
}
