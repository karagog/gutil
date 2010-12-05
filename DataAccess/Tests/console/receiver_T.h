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

#ifndef RECEIVER_T_H
#define RECEIVER_T_H

#include <QObject>
#include "DataAccess/gconsoleiodevice.h"

extern GUtil::DataAccess::GConsoleIODevice *ct;

class receiver_t : public QObject
{
    Q_OBJECT
public:
    receiver_t():QObject(){}
    virtual ~receiver_t(){}

public slots:
    void user_entered_data()
    {
        QString line = ct->ReadLine();
        ct->WriteLine(QString("You just entered \"%1\"\n").arg(line));
    }
};

#endif // RECEIVER_T_H
