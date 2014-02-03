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

#ifndef GUTIL_SELECTDATERANGE_H
#define GUTIL_SELECTDATERANGE_H

#include "gutil_smartpointer.h"
#include "gutil_range.h"
#include <QWidget>
#include <QDateTime>

class QCheckBox;
class QDateEdit;
class QDateTimeEdit;

namespace Ui {
class SelectDateRange;
}

namespace GUtil{ namespace QT{ namespace Controls{


/** Use this control to select a range of time. */
class SelectDateRange :
        public QWidget
{
    Q_OBJECT

    Utils::SmartPointer<Ui::SelectDateRange> ui;

    Utils::SmartPointer<QDateEdit> m_dateEdit;

    Utils::SmartPointer<QWidget> m_customWidget;
    QCheckBox *m_checkStart, *m_checkEnd;
    QDateTimeEdit *m_dateTimeStart, *m_dateTimeEnd;

    QDate m_dateStart, m_dateEnd;
    bool m_suppressUpdates;

public:

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

    explicit SelectDateRange(QWidget *parent = 0);
    explicit SelectDateRange(ComboBoxItemEnum, QWidget *parent = 0);
    ~SelectDateRange();

    /** Sets the given combo box selection.  This affects what date ranges are possible. */
    void SetComboBoxSelection(ComboBoxItemEnum);

    /** Returns the currently selected date range */
    ::GUtil::DataObjects::Range<QDate> GetDateRange() const;

    /** Sets the current date range selection.
     *  The date range may be adjusted to make sense with the combo box selection.
    */
    void SetDateRange(const ::GUtil::DataObjects::Range<QDate> &);


signals:

    /** This signal is emitted to notify that the time range selection has changed.
     *
     *  The start and the end of the time range are given as parameters, but they may
     *  also be null, thus indicating a boundless range.
    */
    void SelectionChanged(QDate range_start, QDate range_end);


private slots:

    // This slot is called whenever the combo box changes or when a date edit changes
    void _update_widgets();

    // Causes the internal date range to be updated from the values in the UI controls
    void _refresh_date_range();


private:

    void _init(ComboBoxItemEnum);

};


}}}

#endif // GUTIL_SELECTDATERANGE_H
