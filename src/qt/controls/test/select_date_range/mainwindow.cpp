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

#include "mainwindow.h"
#include <QVBoxLayout>
#include <QLabel>
USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL2(QT, Controls);

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent),
      m_sdr(new SelectDateRange(this)),
      lbl_dateStart(new QLabel()),
      lbl_dateEnd(new QLabel()),
      lbl_updateCount(new QLabel()),
      m_updateCount(0)
{
    QWidget *c = new QWidget(this);
    this->setCentralWidget(c);
    QVBoxLayout *l = new QVBoxLayout(c);

    l->addWidget(m_sdr);
    l->addWidget(lbl_dateStart);
    l->addWidget(lbl_dateEnd);
    l->addWidget(lbl_updateCount);
    resize(500, 200);

    connect(m_sdr, SIGNAL(SelectionChanged(QDate,QDate)), this, SLOT(DateRangeUpdated(QDate,QDate)));

    QDate d = QDate::currentDate();
    m_sdr->SetDateRange(Range<QDate>::CreateDoubleBound(d, d, true, true));
//    m_sdr->SetDateRange(Range<QDate>::CreateDoubleBound(d.addDays(1 - d.dayOfWeek()),
//                                                        d.addDays(7 - d.dayOfWeek()),
//                                                        true, true));
//    m_sdr->SetDateRange(Range<QDate>::CreateDoubleBound(d.addDays(1 - d.day()),
//                                                        d.addDays(1 - d.day()).addMonths(1),
//                                                        true, false));
//    m_sdr->SetDateRange(Range<QDate>::CreateDoubleBound(d.addDays(1 - d.dayOfYear()),
//                                                        d.addDays(1 - d.dayOfYear()).addYears(1),
//                                                        true, false));

    DateRangeUpdated();
}

MainWindow::~MainWindow()
{

}

void MainWindow::DateRangeUpdated(QDate s, QDate e)
{
    Range<QDate> r( m_sdr->GetDateRange() );

    lbl_dateStart->setText(QString("Start Date: %1").arg(r.LowerBound().Value().toString("ddd MMMM d yyyy")));
    lbl_dateEnd->setText(QString("End Date: %1").arg(r.UpperBound().Value().toString("ddd MMMM d yyyy")));
    lbl_updateCount->setText(QString("Update Count: %1").arg(++m_updateCount));

    GASSERT(s.isNull() || s == r.LowerBound().Value());
    GASSERT(e.isNull() || e == r.UpperBound().Value());
}
