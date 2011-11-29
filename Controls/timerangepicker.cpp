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

#include "timerangepicker.h"
#include "gutil_macros.h"
#include <QVBoxLayout>
#include <QVBoxLayout>
GUTIL_USING_NAMESPACE(Controls);

TimeRangePicker::TimeRangePicker(QWidget *parent)
    :QWidget(parent)
{
    setLayout(new QVBoxLayout(this));

    QWidget *subwidget( new QWidget(this) );
    subwidget->setLayout( new QHBoxLayout(subwidget) );
    subwidget->layout()->setContentsMargins(0,0,0,0);
    subwidget->layout()->addWidget(&ChkStart);
    subwidget->layout()->addWidget(&DteStart);
    layout()->addWidget(subwidget);

    subwidget = new QWidget(this);
    subwidget->setLayout( new QHBoxLayout(subwidget) );
    subwidget->layout()->setContentsMargins(0,0,0,0);
    subwidget->layout()->addWidget(&ChkEnd);
    subwidget->layout()->addWidget(&DteEnd);
    layout()->addWidget(subwidget);

    ChkStart.setText("Start Time");
    ChkEnd.setText("End Time");

    DteStart.setEnabled(false);
    DteEnd.setEnabled(false);

    connect(&ChkStart, SIGNAL(toggled(bool)), this, SLOT(_start_checked(bool)));
    connect(&ChkEnd, SIGNAL(toggled(bool)), this, SLOT(_end_checked(bool)));

    show();
}

GUtil::DataObjects::TimeRange TimeRangePicker::GetRange() const
{
    return GUtil::DataObjects::TimeRange(
                ChkStart.isChecked() ? DteStart.dateTime() : QDateTime(),
                ChkEnd.isChecked() ? DteEnd.dateTime() : QDateTime(),
                1
                );
}

void TimeRangePicker::SetRange(const GUtil::DataObjects::TimeRange &r)
{
    ChkStart.setChecked(!r.LowerBound().isNull());
    ChkEnd.setChecked(!r.UpperBound().isNull());

    if(!r.LowerBound().isNull())
        DteStart.setDateTime(r.LowerBound());
    if(!r.UpperBound().isNull())
        DteEnd.setDateTime(r.UpperBound());
}

void TimeRangePicker::_start_checked(bool c)
{
    DteStart.setEnabled(c);
    if(c)
        DteStart.setDateTime(QDateTime::currentDateTime());
}

void TimeRangePicker::_end_checked(bool c)
{
    DteEnd.setEnabled(c);
    if(c)
        DteEnd.setDateTime(QDateTime::currentDateTime());
}
