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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gutil_selectdaterange.h"
#include <QMainWindow>

class QLabel;

class MainWindow :
        public QMainWindow
{
    Q_OBJECT

    ::GUtil::QT::Controls::SelectDateRange *m_sdr;
    QLabel *lbl_dateStart, *lbl_dateEnd, *lbl_updateCount;

    int m_updateCount;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void DateRangeUpdated(QDate = QDate(), QDate = QDate());

};

#endif // MAINWINDOW_H
