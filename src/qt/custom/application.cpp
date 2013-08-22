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

#include "application.h"
#include "gutil_about.h"
#include <QPluginLoader>
#include <QMessageBox>
NAMESPACE_GUTIL2(QT, Custom);


Application::Application(int &argc, char **argv)
    :QApplication(argc, argv)
{
    _init();
}

Application::Application(int &argc, char **argv, const QString &application_name, const QString &application_version)
    :QApplication(argc, argv)
{
    if(!application_name.isEmpty())
        setApplicationName(application_name);
    if(!application_version.isEmpty())
        setApplicationVersion(application_version);

    _init();
}

void Application::_init()
{
    connect(this, SIGNAL(aboutToQuit()), this, SLOT(about_to_quit()));
}

Application::~Application()
{}

bool Application::notify(QObject *o, QEvent *ev)
{
    bool ret(false);
    try{
        ret = QApplication::notify(o, ev);
    }
    catch(const Exception<> &ex){
        handle_exception(ex);
    }
    catch(const std::exception &ex){
        handle_std_exception(ex);
    }
    return ret;
}

void Application::Quit()
{
    Exit();
}

void Application::About(QWidget *parent)
{
    QMessageBox::information(parent,
        QString("About %1").arg(applicationName()),
        QString("Version: %1").arg(applicationVersion()));
}

void Application::AboutGUtil(QWidget *dialog_parent)
{
    ::GUtil::QT::UI::About::ShowAboutGUtil(dialog_parent);
}

void Application::about_to_quit(){}


END_NAMESPACE_GUTIL2;
