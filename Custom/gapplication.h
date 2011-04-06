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

#ifndef GAPPLICATION_H
#define GAPPLICATION_H

#include <QApplication>
#include "Core/Utils/commandlineargs.h"

namespace GUtil{
namespace Core{
    class Exception;
}


namespace Custom{


class GApplication :
        public QApplication
{
    Q_OBJECT
public:

    explicit GApplication(int &argc, char **argv);
    virtual ~GApplication(){}

    // Get convenient access to the command line arguments
    inline Core::Utils::CommandLineArgs Args() const{
        return Core::Utils::CommandLineArgs(argc(), argv());
    }

    virtual bool notify(QObject *, QEvent *);


protected slots:

    // You must put cleanup code in here, or connect your own handler to the
    //  'aboutToQuit' signal
    virtual void cleanup(){}

    // You can override these methods, which are called in the event of an exception
    //  during an application event
    virtual void handle_exception(const Core::Exception &){}
    virtual void handle_std_exception(const std::exception &){}

};


}}

#endif // GAPPLICATION_H
