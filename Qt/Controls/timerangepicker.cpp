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

#include "timerangepicker.h"
#include "gutil_macros.h"
#include <QVBoxLayout>
#include <QVBoxLayout>
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL2(QT, Controls);


TimeRangePicker::TimeRangePicker(QWidget *parent)
    :QWidget(parent)
{
    setLayout(new QVBoxLayout(this));

    QWidget *subwidget( new QWidget(this) );
    subwidget->setLayout( new QHBoxLayout(subwidget) );
    subwidget->layout()->setContentsMargins(0,0,0,0);
    subwidget->layout()->addWidget(&StartCheckbox);
    subwidget->layout()->addWidget(&StartDateEdit);
    layout()->addWidget(subwidget);

    subwidget = new QWidget(this);
    subwidget->setLayout( new QHBoxLayout(subwidget) );
    subwidget->layout()->setContentsMargins(0,0,0,0);
    subwidget->layout()->addWidget(&EndCheckbox);
    subwidget->layout()->addWidget(&EndDateEdit);
    layout()->addWidget(subwidget);

    StartCheckbox.setText("Start Time");
    EndCheckbox.setText("End Time");

    StartDateEdit.setEnabled(false);
    EndDateEdit.setEnabled(false);

    connect(&StartCheckbox, SIGNAL(toggled(bool)), this, SLOT(_start_checked(bool)));
    connect(&EndCheckbox, SIGNAL(toggled(bool)), this, SLOT(_end_checked(bool)));

    show();
}

Range<QDateTime> TimeRangePicker::GetRange() const
{
    Range<QDateTime> ret;
    if(StartCheckbox.isChecked())
        ret.SetLowerBound(StartDateEdit.dateTime(), true);
    if(EndCheckbox.isChecked())
        ret.SetUpperBound(EndDateEdit.dateTime(), false);
    return ret;
}

void TimeRangePicker::SetRange(const Range<QDateTime> &r)
{
    StartCheckbox.setChecked(r.HasLowerBound());
    EndCheckbox.setChecked(r.HasUpperBound());

    if(r.HasLowerBound())
        StartDateEdit.setDateTime(r.LowerBound().Value());
    if(r.HasUpperBound())
        EndDateEdit.setDateTime(r.UpperBound().Value());
}


END_NAMESPACE_GUTIL2;
