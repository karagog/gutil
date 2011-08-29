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
GUTIL_USING_NAMESPACE(Custom);

GApplicationBase *GApplicationBase::GlobalInstance(0);

GApplicationBase::GApplicationBase()
{
    if(GlobalInstance)
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "A GApplicationBase has already been initialized.");

    GlobalInstance = this;
}

GApplicationBase::~GApplicationBase()
{
    if(this == GlobalInstance)
        GlobalInstance = 0;
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

void GApplicationBase::Exit(int return_code)
{
    // Derived classes will execute their own cleanup code when the application exits
    gApp->application_exiting();

    QCoreApplication::exit(return_code);
}

void GApplicationBase::application_exiting()
{
    // We put this here, rather than in Exit(), because we force the use to call the base
    //  implementation of this function, which is necessary to conduct the cleanup of the
    //  cleanup objects.  We want every level of subclassing of this class to have the
    //  opportunity to cleanup their own memory, so for correctness of the implementation
    //  everybody must call the base implementation.
    while(_cleanup_objects.count() > 0)
        delete _cleanup_objects.pop();
}

void GApplicationBase::handle_exception(const GUtil::Core::Exception &ex)
{
    throw ex;
}

void GApplicationBase::handle_std_exception(const std::exception &ex)
{
    throw ex;
}

GApplicationBase::CleanupObject::~CleanupObject()
{

}

GUtil::Core::Utils::CommandLineArgs GApplicationBase::Args() const
{
    return Core::Utils::CommandLineArgs(qApp->argc(), qApp->argv());
}
