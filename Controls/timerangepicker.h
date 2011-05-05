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

#ifndef TIMERANGEPICKER_H
#define TIMERANGEPICKER_H

#include "DataObjects/timeregion.h"
#include <QWidget>
#include <QCheckBox>
#include <QDateTimeEdit>

namespace GUtil{ namespace Controls{


class TimeRangePicker :
        public QWidget
{
    Q_OBJECT
public:

    explicit TimeRangePicker(QWidget *parent = 0);

    DataObjects::TimeRange GetRange() const;
    void SetRange(const DataObjects::TimeRange &);

    // The elements of this control will be open to the public so they can customize them
    QCheckBox ChkStart;
    QCheckBox ChkEnd;
    QDateTimeEdit DteStart;
    QDateTimeEdit DteEnd;


private slots:

    void _start_checked(bool);
    void _end_checked(bool);

};


}}

#endif // TIMERANGEPICKER_H
