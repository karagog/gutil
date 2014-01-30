/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "console_repeater.h"
#include <QCoreApplication>

console_repeater::console_repeater(QObject *parent)
    :QObject(parent)
{
    connect(&_cio, SIGNAL(ReadyRead()), this, SLOT(new_data()));
    connect(this, SIGNAL(repeat(QString)), &_cio, SLOT(WriteLine(QString)));
}

void console_repeater::new_data()
{
    QString s( _cio.ReadLine() );
    if(s == "q")
        qApp->quit();
    else
        emit repeat( s );
}
