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

#ifndef CONSOLE_REPEATER_H
#define CONSOLE_REPEATER_H

#include "gutil_consoleiodevice.h"
#include <QObject>

class console_repeater : public QObject
{
    Q_OBJECT

    GUtil::QT::ConsoleIODevice _cio;

public:

    explicit console_repeater(QObject *parent = 0);


signals:

    void repeat(const QString &);


public slots:

    void new_data();

};

#endif // CONSOLE_REPEATER_H
