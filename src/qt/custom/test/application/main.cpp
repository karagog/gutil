/*Copyright 2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <QCoreApplication>
#include "gutil_coreapplication.h"
#include <signal.h>
USING_NAMESPACE_GUTIL;
USING_NAMESPACE_GUTIL1(QT);


class my_application :
        public CoreApplication
{
public:
    my_application(int &argc, char **argv)
        :CoreApplication(argc, argv)
    {
        // Note: Use this PID to send signals via the 'kill -<signal>' function.
        //  Use kill -l to list the possible signal numbers.
        printf("Starting application PID: %d\n", applicationPid());
    }

protected:

    virtual void handle_os_signal(int sig_num)
    {
        printf("Handled signal: %d\n", sig_num);
    }

};


int main(int argc, char *argv[])
{
    my_application a(argc, argv);

    a.IgnoreSignalFromOS(SIGINT);
    a.HandleSignalFromOS(SIGHUP);
    a.HandleSignalFromOS(SIGPWR);

    return a.exec();
}
