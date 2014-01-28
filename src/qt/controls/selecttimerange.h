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

#ifndef GUTIL_SELECTTIMERANGE_H
#define GUTIL_SELECTTIMERANGE_H

#include "gutil_smartpointer.h"
#include "gutil_range.h"
#include <QWidget>
#include <QDateTime>

class QCheckBox;
class QDateEdit;
class QDateTimeEdit;

namespace Ui {
class SelectTimeRange;
}

namespace GUtil{ namespace QT{ namespace Controls{


/** Use this control to select a range of time. */
class SelectTimeRange :
        public QWidget
{
    Q_OBJECT

    Utils::SmartPointer<Ui::SelectTimeRange> ui;

    Utils::SmartPointer<QDateEdit> m_dateEdit;

    Utils::SmartPointer<QWidget> m_customWidget;
    QCheckBox *m_checkStart, *m_checkEnd;
    QDateTimeEdit *m_dateTimeStart, *m_dateTimeEnd;

public:

    explicit SelectTimeRange(QWidget *parent = 0);
    ~SelectTimeRange();

    /** Enumerates the indices of the combo box. */
    enum ComboBoxItemEnum
    {
        Day = 0,
        Week = 1,
        Month = 2,
        Year = 3,
        AllTime = 4,
        Custom = 5
    };

    ::GUtil::DataObjects::Range<QDateTime> GetSelection() const;


signals:

    /** This signal is emitted to notify that the time range selection has changed.
     *
     *  The start and the end of the time range are given as parameters, but they may
     *  also be null, thus indicating a boundless range.
    */
    void SelectionChanged(QDateTime range_start, QDateTime range_end);


private slots:

    void _combobox_index_changed(int);

};


}}}

#endif // GUTIL_SELECTTIMERANGE_H
