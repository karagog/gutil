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
#include "gutil_globals.h"
#include "Core/exception.h"
#include "Ui/aboutgutil.h"
GUTIL_USING_NAMESPACE(GUICustom);
GUTIL_USING_NAMESPACE(Custom);
GUTIL_USING_NAMESPACE(BusinessObjects);

GApplication::GApplication(int &argc, char **argv, const QString &app_name, const QString &app_version)
    :QApplication(argc, argv)
{
    if(!app_name.isNull())
        setApplicationName(app_name);
    if(!app_version.isNull())
        setApplicationVersion(app_version);

    // Initialize this after setting the application name and version
    m_psi = new ProcessStatusIndicator(this);
}

GApplication::~GApplication()
{

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

void GApplication::AboutGUtil()
{
    GUtil::UI::AboutGUtil().exec();
}

void GApplication::Quit()
{
    gApp->Exit();
}
