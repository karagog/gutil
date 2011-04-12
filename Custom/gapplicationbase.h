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

namespace GUtil{ namespace Custom{


// Used as a base class for the common functionality of GApplication
//  and GCoreApplication.
class GApplicationBase
{
public:

    // Get convenient access to the command line arguments
    inline Core::Utils::CommandLineArgs Args() const{
        return Core::Utils::CommandLineArgs(m_app->argc(), m_app->argv());
    }

    // You can have the application object cleanup objects in the cleanup handler
    //  rather than the destructor by using this interface.  Classes that
    //  want to be cleaned up must derive from CleanupObject
    class CleanupObject{
    public:
        virtual ~CleanupObject(){}
    };
    void AddCleanupObject(CleanupObject *o);
    void RemoveCleanupObject(CleanupObject *o);
    inline QList<CleanupObject*> CleanupObjects() const{ return _cleanup_objects; }

    // Tells the GApplication to cleanup (called automatically when the application
    //  is about to quit)
    void Cleanup();

    ~GApplicationBase();

protected:

    // This is an abstract class; it expects to be derived by a derivative of QCoreApplication
    explicit GApplicationBase(QCoreApplication *);

    // Subclasses can override this to make their own cleanup code, which is in
    //  addition to this class' cleanup code
    virtual void cleanup_protected(){}

    // You can override these methods, which are called in the event of an exception
    //  during an application event
    virtual void handle_exception(const Core::Exception &){}
    virtual void handle_std_exception(const std::exception &){}


private:

    QList<CleanupObject *> _cleanup_objects;
    QCoreApplication *m_app;

};


// This class is just a facade; it is here because GApplicationBase cannot be
//  a QObject itself, because it is expected that it will be derived by other
//  derivitives of QObject
class GApplicationCleanerUpper :
        public QObject
{
    Q_OBJECT
public:
    explicit GApplicationCleanerUpper(GApplicationBase *ga)
        :m_gapp(ga) {}
public slots:
    void cleanup(){
        m_gapp->Cleanup();
    }
private:
    GApplicationBase *m_gapp;
};


}}

#endif // GAPPLICATIONBASE_H
