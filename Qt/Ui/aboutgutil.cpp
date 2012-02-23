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
#include "Core/globals.h"

NAMESPACE_GUTIL2(QT, UI);


AboutGUtil::AboutGUtil(QWidget *p)
    :About(p, false)
{
    Title.setText(QString("GUtil Utility Library - %1")
                       .arg(GUTIL_VERSION));

    BuildInfo.setText(QString("Built on %1 at %2").arg(__DATE__).arg(__TIME__));

    Text.setPlainText("GUtil is a library of useful functions that is designed to make deveopment"
                      " much easier.  By encapsulating complex and tedious functions"
                      " it gives you a toolbox of objects you can use right away to"
                      " develop more complex, high-level behaviors."
                      "\n\n"

                      "It was developed entirely by George Karagoulis, mostly while he was "
                      " in the process of developing other open-source initiatives.  Whenever"
                      " he identified a useful piece of code he added it to the ever-growing"
                      " library to make future applications go more quickly."
                      "\n\n"

                      "The library is protected by the Apache license 2.0, details of which can be "
                      " found in the software package that built this application.");

    //  SetImage("Some Image in the Future?");
}


END_NAMESPACE_GUTIL2;
