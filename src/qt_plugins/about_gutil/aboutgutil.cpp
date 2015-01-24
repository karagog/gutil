/*Copyright 2010-2015 George Karagoulis

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
#include <gutil/globals.h>
#include <gutil/about.h>
#include <QResource>


/** Defines the implementation of the about window */
class __about_gutil :
        public GUtil::Qt::About
{
    Q_OBJECT
public:

    explicit __about_gutil(QWidget *parent)
        : GUtil::Qt::About(parent, false, true)
    {
        // Set up the about window
        _dialog.setWindowTitle("About GUtil");
        _dialog.resize(500, 375);

        _header.setText("GUtil Libraries - " GUTIL_VERSION);

        _buildinfo.setText(QString("Built on %1").arg(GUtil::BUILD_TIME));

        // Fetch the about text from a resource
        QResource r(":/GUtil/About/abouttext.txt");
        if(r.isValid())
            _text.setPlainText(r.isCompressed() ?
                                  qUncompress(r.data(), r.size()) :
                                  QByteArray(reinterpret_cast<const char *>(r.data()), r.size()));
        else
            _text.setPlainText("No Text");
    }

};




NAMESPACE_GUTIL1(Qt);


AboutGUtil::AboutGUtil()
    :ptr(NULL)
{}

void AboutGUtil::ShowAboutGUtil(QWidget *parent)
{
    __about_gutil *ag = (__about_gutil*)ptr;
    if(!ptr)
        ptr = ag = (new __about_gutil(parent));
    ag->ShowAbout();
}


END_NAMESPACE_GUTIL1;

#include "aboutgutil.moc"
