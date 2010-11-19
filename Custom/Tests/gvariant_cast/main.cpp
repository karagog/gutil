/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <QtCore/QCoreApplication>
#include <QDateTime>
#include "Custom/gvariant.h"
using namespace GUtil::Custom;

int main(int argc, char *argv[])
{
    GVariant v = 5;
    int test = (int)v;

    v = 2.55623;
    double t = (double)v;

    v = true;
    bool b = (bool)v;

    v = "Hello World";
    QString probe = (QString)v;

    v = QDateTime();
    QDateTime dt = (QDateTime)v;

    v = QTime(1,2,3,4);
    QTime tm = (QTime)v;
}
