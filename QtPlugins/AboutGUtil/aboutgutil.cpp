/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "aboutgutil.h"
#include "gutil_about.h"
#include "gutil_globals.h"
#include <QResource>

NAMESPACE_GUTIL2(QT, Plugins);

void AboutGUtil::ShowAboutGUtil(QWidget *parent)
{
    UI::About a(parent, false);

    a.Title.setText("GUtil Libraries - " GUTIL_VERSION);

    // Note: GUTIL_BUILD_DATE is defined by one of our QMake .pro files
    a.BuildInfo.setText("Built on " GUTIL_BUILD_DATE);

    // Fetch the about text from a resource
    QResource r(":/GUtil/About/abouttext.txt");
    if(r.isValid())
        a.Text.setPlainText(r.isCompressed() ?
                              qUncompress(r.data(), r.size()) :
                              QByteArray(reinterpret_cast<const char *>(r.data()), r.size()));
    else
        a.Text.setPlainText("No Text");

    a.exec();
}


END_NAMESPACE_GUTIL2;

Q_EXPORT_PLUGIN2(GUtilAboutPlugin, GUtil::QT::Plugins::AboutGUtil)
