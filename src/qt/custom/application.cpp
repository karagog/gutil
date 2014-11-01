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

#include "application.h"
#include <gutil/about.h>
#include <QPluginLoader>
#include <QMessageBox>
#include <QAction>
#include <memory>
using namespace std;
NAMESPACE_GUTIL1(Qt);


Application::Application(int &argc, char **argv)
    :QApplication(argc, argv),
      ApplicationBase(argc, argv)
{
    _init();
}

Application::Application(int &argc, char **argv, const QString &application_name, const QString &application_version)
    :QApplication(argc, argv),
      ApplicationBase(argc, argv)
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
    catch(exception &ex){
        handle_exception(ex);
    }
    catch(const shared_ptr<GUtil::Exception<>> &ex){
        handle_exception(*ex);
    }
    return ret;
}

void Application::Quit()
{
    Exit();
}

void Application::About(QWidget *p)
{
    Application *a = gApp;
    if(a)
        a->show_about(p);
}

QAction *Application::CreateActionQuit(QObject *p)
{
    QAction *ret = NULL;
    Application *a = gApp;
    if(a){
        ret = new QAction(tr("&Quit"), p);
        connect(ret, SIGNAL(triggered()), a, SLOT(Quit()));
    }
    return ret;
}

void Application::show_about(QWidget *parent)
{
    QMessageBox::information(parent,
        QString("About %1").arg(applicationName()),
        QString("Version: %1").arg(applicationVersion()));
}

QAction *Application::CreateActionAbout(QObject *p)
{
    QAction *ret = NULL;
    Application *a = gApp;
    if(a){
        ret = new QAction(tr("&About"), p);
        connect(ret, SIGNAL(triggered()), a, SLOT(About()));
    }
    return ret;
}

QAction *Application::CreateActionAboutGUtil(QObject *p)
{
    QAction *ret = NULL;
    Application *a = gApp;
    if(a){
        ret = new QAction(tr("About GUtil"), p);
        connect(ret, SIGNAL(triggered()), a, SLOT(AboutGUtil()));
    }
    return ret;
}

void Application::AboutGUtil(QWidget *dialog_parent)
{
    GUtil::Qt::About::ShowAboutGUtil(dialog_parent);
}

void Application::about_to_quit(){}


END_NAMESPACE_GUTIL1;
