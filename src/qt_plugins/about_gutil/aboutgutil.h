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

#ifndef GUTIL_ABOUTGUTIL_H
#define GUTIL_ABOUTGUTIL_H

#include <gutil/aboutplugininterface.h>
#include <QtPlugin>

namespace GUtil{ namespace Qt{


/** Plugin wrapper that shows GUtil's about window

    I did this in a plugin so that any text/images will be loaded only
    when needed.
*/
class AboutGUtil :
        public QObject,
        public IAboutGUtil
{
    Q_OBJECT
    Q_INTERFACES(GUtil::Qt::IAboutGUtil)
    Q_PLUGIN_METADATA(IID "GUtil.Qt.AboutGUtil")
public:

    /** Shows the "About GUtil" window with the widget as parent */
    virtual void ShowAboutGUtil(QWidget *parent = 0);

};


}}

#endif // GUTIL_ABOUTGUTIL_H
