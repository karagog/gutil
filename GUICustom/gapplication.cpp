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

GApplication::GApplication(int &argc, char **argv)
    :QApplication(argc, argv)
{}

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
