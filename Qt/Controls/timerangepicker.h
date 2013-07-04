/*Copyright 2010-2013 George Karagoulis

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

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include "gutil_range.h"
#include <QWidget>
#include <QCheckBox>
#include <QDateTimeEdit>

namespace GUtil{ namespace QT{ namespace Controls{


/** Implements a control that facilitates a time range input from the user. */
class TimeRangePicker :
        public QWidget
{
    Q_OBJECT
public:

    explicit TimeRangePicker(QWidget *parent = 0);

    DataObjects::Range<QDateTime> GetRange() const;
    void SetRange(const DataObjects::Range<QDateTime> &);

    // The elements of this control will be open to the public so they can customize them
    QCheckBox StartCheckbox;
    QCheckBox EndCheckbox;
    QDateTimeEdit StartDateEdit;
    QDateTimeEdit EndDateEdit;


private slots:

    void _start_checked(bool c){
        StartDateEdit.setEnabled(c);
        if(c) StartDateEdit.setDateTime(QDateTime::currentDateTime());
    }
    void _end_checked(bool c){
        EndDateEdit.setEnabled(c);
        if(c) EndDateEdit.setDateTime(QDateTime::currentDateTime());
    }

};


}}}


#endif // GUI_FUNCTIONALITY

#endif // TIMERANGEPICKER_H
