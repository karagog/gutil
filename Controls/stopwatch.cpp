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

#include "stopwatch.h"
#include "gutil_macros.h"
#include <QTimerEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
GUTIL_USING_NAMESPACE(Controls);

#define TIMER_INTERVAL 100


Stopwatch::Stopwatch(QWidget *parent)
    :QWidget(parent),
      m_resolution(HundredMilliseconds)
{
    setLayout(new QVBoxLayout(this));

    QWidget *tmp( new QWidget(this) );
    QFormLayout *fl( new QFormLayout(tmp) );
    tmp->setLayout(fl);
    fl->addRow("Start:", lbl_startTime = new QLabel(this));
    fl->addRow("Stop:", lbl_endTime = new QLabel(this));
    lbl_startTime->setAlignment(Qt::AlignRight);
    lbl_endTime->setAlignment(Qt::AlignRight);
    layout()->addWidget(tmp);

    layout()->addWidget(lbl_time = new QLabel(this));
    lbl_time->setAlignment(Qt::AlignHCenter);
    {
        QFont f;
        f.setBold(true);
        lbl_time->setFont(f);
    }
    {
        QSizePolicy pol(QSizePolicy::Expanding, QSizePolicy::Expanding);
        pol.setVerticalStretch(1);
        lbl_time->setSizePolicy(pol);
    }

    tmp = new QWidget(this);
    tmp->setLayout(new QHBoxLayout(tmp));
    tmp->layout()->addWidget(btn_reset = new QPushButton(this));
    tmp->layout()->addWidget(btn_start = new QPushButton(this));

    btn_reset->setText("Reset");
    btn_start->setText("Start/Stop");
    btn_start->setCheckable(true);

    layout()->addWidget(tmp);

    connect(btn_start, SIGNAL(clicked(bool)), this, SLOT(StartStop(bool)));
    connect(btn_reset, SIGNAL(clicked()), &_stopwatch, SLOT(Reset()));

    connect(&_stopwatch, SIGNAL(NotifyTimeChanged()),
            this, SLOT(_update_label()));

    _update_label();
}

#define DATETIME_FORMAT "MMM d, yyyy h:mm:ss ap"

void Stopwatch::_update_label()
{
    lbl_startTime->setText(TimeStart().isNull() ?
                               QString::null :
                               TimeStart().toString(DATETIME_FORMAT));
    lbl_endTime->setText(TimeStop().isNull() ?
                             QString::null :
                             TimeStop().toString(DATETIME_FORMAT));

    QString fmt("Elapsed:    %1 : %2 : %3");
    int hours(0), minutes(0), seconds(0), milseconds(0);

    qint64 diff = _stopwatch.TimeStart().msecsTo(_stopwatch.TimeEnd());
    hours = diff / (1000 * 60 * 60);
    diff -= hours * (1000 * 60 * 60);

    minutes = diff / (1000 * 60);
    diff -= minutes * (1000 * 60);

    seconds = diff / 1000;
    diff -= seconds * 1000;

    milseconds = diff;

    fmt = fmt
            .arg(hours)
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));

    switch(m_resolution)
    {
    case Milliseconds:
        fmt += QString(" : %1").arg(milseconds, 3, 10, QChar('0'));
        break;
    case HundredMilliseconds:
        fmt += QString(" : %1").arg(milseconds / 100);
        break;
    default:
        break;
    }

    lbl_time->setText(fmt);
}

void Stopwatch::SetResolution(ResolutionEnum r)
{
    int res;
    switch(r)
    {
    case Milliseconds:
        res = 1;
        break;
    case HundredMilliseconds:
        res = 100;
        break;
    case Seconds:
        res = 1000;
        break;
    default:
        return;
    }

    m_resolution = r;
    _stopwatch.SetTimerResolution(res);
}

void Stopwatch::Start()
{
    StartStop(true);
}

void Stopwatch::Stop()
{
    StartStop(false);
}

void Stopwatch::StartStop(bool start)
{
    _stopwatch.StartStop(start);

    _update_label();
}

void Stopwatch::Reset()
{
    _stopwatch.Reset();

    _update_label();
}
