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

#include "stopwatch.h"
#include "gutil_macros.h"
#include <QTimerEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFormLayout>

NAMESPACE_GUTIL2(QT, Controls);


#define TIMER_INTERVAL 100


Stopwatch::Stopwatch(QWidget *parent)
    :QWidget(parent)
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
    connect(btn_reset, SIGNAL(clicked()), this, SLOT(Reset()));

    connect(&_stopwatch, SIGNAL(NotifyRefreshed(QDateTime)),
            this, SLOT(_update_label(QDateTime)));

    SetResolution(HundredMilliseconds);
    _update_label();
}

#define DATETIME_FORMAT "MMM d, yyyy h:mm:ss ap"

void Stopwatch::_update_label(const QDateTime &time)
{
    QDateTime start(TimeStart()), stop(TimeStopped());
    lbl_startTime->setText(start.isNull() ?
                               QString::null :
                               start.toString(DATETIME_FORMAT));
    lbl_endTime->setText(stop.isNull() ?
                             QString::null :
                             stop.toString(DATETIME_FORMAT));

    int hour(0), minute(0), second(0), msecond (0);
    if(time.isValid())
    {
        hour = time.time().hour();
        minute = time.time().minute();
        second = time.time().second();
        msecond = time.time().msec();
    }

    QString fmt( QString("Elapsed:    %1 : %2 : %3")
                 .arg(hour)
                 .arg(minute, 2, 10, QChar('0'))
                 .arg(second, 2, 10, QChar('0')));

    switch(m_resolution)
    {
    case Milliseconds:
        fmt += QString(" : %1").arg(msecond, 3, 10, QChar('0'));
        break;
    case HundredMilliseconds:
        fmt += QString(" : %1").arg(msecond / 100);
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
    _stopwatch.SetAutoRefreshTime(res);
}


END_NAMESPACE_GUTIL2;
