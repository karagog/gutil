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

#ifndef GAPPLICATIONBASE_H
#define GAPPLICATIONBASE_H

#include "gutil_macros.h"
#include "Core/Utils/commandlineargs.h"
#include "Core/exception.h"
#include <QCoreApplication>
#include <QStack>

namespace GUtil{ namespace Custom{


// Used as a base class for the common functionality of GApplication
//  and GCoreApplication.  Provides a memory management stack, onto which
//  you can push any kind of memory, and it will be deleted in FILO order
class GApplicationBase
{
public:

    // Use this function to quit the application, instead of QApplication::quit.
    //  This will call 'application_exiting()', which you can override to put cleanup code in
    static void Exit();

    // Get convenient access to the command line arguments
    inline Core::Utils::CommandLineArgs Args() const{
        return Core::Utils::CommandLineArgs(qApp->argc(), qApp->argv());
    }

    // You can have the application object cleanup objects in the cleanup handler
    //  rather than the destructor by using this interface.  Classes that
    //  want to be cleaned up must derive from CleanupObject
    class CleanupObject{
    public:
        virtual ~CleanupObject();
    };

    void AddCleanupObject(CleanupObject *o);
    void RemoveCleanupObject(CleanupObject *o);

    inline QStack<CleanupObject *> CleanupObjects() const{ return _cleanup_objects; }


protected:

    // Subclasses can override this to make their own cleanup code
    virtual void application_exiting();

    // You can override these methods, which are called in the event of an exception
    //  during an application event.  Don't call the base implementation, as it will
    //  only throw the exception again.
    virtual void handle_exception(const Core::Exception &);
    virtual void handle_std_exception(const std::exception &);


private:

    QStack<CleanupObject *> _cleanup_objects;

};


}}


// A reference to the global instance of GApplicationBase
#define gApp ((GUtil::Custom::GApplicationBase *)qApp)


#endif // GAPPLICATIONBASE_H
