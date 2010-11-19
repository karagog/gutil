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
#include "Custom/gvariant.h"
using namespace GUtil::Custom;

int main(int argc, char *argv[])
{
    GVariant v = 5;
    int test = v;

    v = "Hello World!";
    const char *c = v;

    v = 2.55623;
    double t = v;

    v = true;
    bool b = v;

    v = "Hello World";
    QString probe = v;

    v = QDateTime();
    QDateTime dt = v;

    v = QTime(1,2,3,4);
    QTime tm = v;

    v = QStringList("Hi");
    QStringList sl = v;

    v = QVariantList() << "hi";
    QVariantList vl = v;

    v = QRegExp("HI");
    QRegExp re = v;

    v = QBitArray(5);
    QBitArray ba = v;

    v = QByteArray("HI");
    QByteArray bya = v;

    v = QRect(1, 2, 3, 4);
    QRect r = v;

    v = QSize(1, 2);
    QSize sz = v;

    v = QVariantMap();
    QVariantMap vm = v;

    v = QUrl("http://google.com");
    QUrl u = v;

    v = std::string("hi");
    std::string std_s = v;

    v = QChar('h');
    QChar ch = v;
}
