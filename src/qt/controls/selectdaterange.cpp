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

#include "selectdaterange.h"
#include "ui_selectdaterange.h"
#include "gutil_macros.h"
#include <QDateEdit>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QGridLayout>
#include <QFormLayout>
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(QT);


SelectDateRange::SelectDateRange(QWidget *parent)
    :QWidget(parent)
{
    _init(Day);
}

SelectDateRange::SelectDateRange(ComboBoxItemEnum c, QWidget *parent)
    :QWidget(parent)
{
    _init(c);
}

void SelectDateRange::_init(ComboBoxItemEnum c)
{
    ui = new Ui::SelectDateRange;
    m_dayFormat = "MMM dd, yyyy";
    m_suppressUpdates = false;

    ui->setupUi(this);

    SetComboBoxSelection(c);
    _combobox_index_changed();
}

SelectDateRange::~SelectDateRange()
{}

Range<QDate> SelectDateRange::GetDateRange() const
{
    if(m_dateStart.isNull() && m_dateEnd.isNull())
        return Range<QDate>::Universe();
    else if(m_dateStart.isNull())
        return Range<QDate>::CreateUpperBound(m_dateEnd, true);
    else if(m_dateEnd.isNull())
        return Range<QDate>::CreateLowerBound(m_dateStart, true);
    else
        return Range<QDate>::CreateDoubleBound(m_dateStart, m_dateEnd, true, true);
}

void SelectDateRange::_refresh_date_range()
{
    if(m_suppressUpdates)
        return;

    QDate new_start, new_end;
    switch((ComboBoxItemEnum)ui->comboBox->currentIndex())
    {
    case Day:
        new_start = m_dateEdit->date();
        new_end = new_start;
        break;
    case Week:
    {
        QDate d = m_dateEdit->date();
        new_start = d.addDays(1 - d.dayOfWeek());
        new_end = new_start.addDays(6);
    }
        break;
    case Month:
    {
        QDate d = m_dateEdit->date();
        new_start = d.addDays(1 - d.day());
        new_end = new_start.addMonths(1).addDays(-1);
    }
        break;
    case Year:
    {
        QDate d = m_dateEdit->date();
        new_start = d.addDays(1 - d.dayOfYear());
        new_end = new_start.addYears(1).addDays(-1);
    }
        break;
    case AllTime:
        break;
    case Custom:
        // Grab the range from the custom widget
        if(m_checkStart->isChecked()){
            new_start = m_dateTimeStart->date();
        }
        if(m_checkEnd->isChecked()){
            new_end = m_dateTimeEnd->date();
        }
        break;
    default:
        break;
    }


    // Only update when either value changed
    if(!(m_dateStart == new_start &&
         m_dateEnd == new_end))
    {
        m_dateStart = new_start;
        m_dateEnd = new_end;

        emit SelectionChanged(m_dateStart, m_dateEnd);
    }
}

// Adjusts the date depending on whether the bound includes the given date
void __apply_new_bound(QDate &mem, const Bound<QDate> &b, bool lower_bound)
{
    mem = b.Value();
    if((lower_bound && !b.ValueIncludedInBound()) ||
            (!lower_bound && b.ValueIncludedInBound()))
        mem = mem.addDays(1);
}

void SelectDateRange::SetDateRange(const Range<QDate> &r)
{
    QDate new_start, new_end;

    if(!r.LowerBound().Value().isNull()){
        __apply_new_bound(new_start, r.LowerBound(), r.LowerBound().ValueIncludedInBound());
    }

    if(!r.UpperBound().Value().isNull()){
        __apply_new_bound(new_end, r.UpperBound(), r.UpperBound().ValueIncludedInBound());
    }


    // Try to apply a logical combo box setting, given the date range.
    //  i.e. If the range is 1 day, select a day, etc...
    m_suppressUpdates = true;
    if(new_start.isNull() && new_end.isNull()){
        m_dateStart = QDate();
        m_dateEnd = QDate();
        SetComboBoxSelection(AllTime);
    }
    else if(new_start.isNull() || new_end.isNull()){
        m_dateStart = new_start;
        m_dateEnd = new_end;
        SetComboBoxSelection(Custom);
        if(m_dateStart.isNull())
            m_dateTimeEnd->setDate(m_dateEnd);
        else
            m_dateTimeStart->setDate(m_dateStart);
    }
    else
    {
        int days_apart = new_start.daysTo(new_end);
        if(0 > days_apart)
            THROW_NEW_GUTIL_EXCEPTION2(Exception, "The end date must come after the start");

        m_dateStart = new_start;
        m_dateEnd = new_end;

        if(0 == days_apart){
            SetComboBoxSelection(Day);
            m_dateEdit->setDate(m_dateStart);
        }
        else if(6 == days_apart && 1 == m_dateStart.dayOfWeek()){
            SetComboBoxSelection(Week);
            m_dateEdit->setDate(m_dateStart);
        }
        else if(1 == m_dateStart.day() && 1 == m_dateEnd.day() &&
                0 == GUtil::FuzzyCompare(days_apart, 29, 2))
        {
            SetComboBoxSelection(Month);
            m_dateEdit->setDate(m_dateStart);
        }
        else if(1 == m_dateStart.day() && m_dateEnd == m_dateStart.addYears(1).addDays(-1))
        {
            SetComboBoxSelection(Year);
            m_dateEdit->setDate(m_dateStart);
        }
        else{
            SetComboBoxSelection(Custom);
            m_dateTimeStart->setDate(m_dateStart);
            m_dateTimeEnd->setDate(m_dateEnd);
        }
    }
    m_suppressUpdates = false;
}

void SelectDateRange::_combobox_index_changed()
{
    int ind = ui->comboBox->currentIndex();

    // First update the control layout:
    switch((ComboBoxItemEnum)ind)
    {
    case Day:
    case Week:
    case Month:
    case Year:
    case AllTime:
        if(m_checkStart){
            ui->gridLayout->removeWidget(m_checkStart);
            ui->gridLayout->removeWidget(m_checkEnd);
            ui->gridLayout->removeWidget(m_dateTimeStart);
            ui->gridLayout->removeWidget(m_dateTimeEnd);
            m_checkStart->hide();
            m_checkEnd->hide();
            m_dateTimeStart->hide();
            m_dateTimeEnd->hide();
        }

        if(!m_dateEdit){
            m_dateEdit = new QDateEdit;
            m_dateEdit->setCalendarPopup(true);
            QSizePolicy sp;
            sp.setHorizontalPolicy(QSizePolicy::Expanding);
            sp.setHorizontalStretch(1);
            m_dateEdit->setSizePolicy(sp);
            connect(m_dateEdit, SIGNAL(dateTimeChanged(QDateTime)),
                    this, SLOT(_date_updated()));
            //m_dateEdit->setAlignment(Qt::AlignRight);
        }

        ui->gridLayout->addWidget(m_dateEdit, 0, 2, 1, 2);

        m_dateEdit->setEnabled(AllTime != ind);
        m_dateEdit->show();

        break;
    case Custom:
        if(m_dateEdit){
            ui->gridLayout->removeWidget(m_dateEdit);
            m_dateEdit->hide();
        }

        if(!m_checkStart)
        {
            m_checkStart = new QCheckBox(tr("Date Start"));
            m_checkEnd = new QCheckBox(tr("Date End"));
            m_dateTimeStart = new QDateTimeEdit;
            m_dateTimeStart->setCalendarPopup(true);
            connect(m_dateTimeStart, SIGNAL(dateTimeChanged(QDateTime)),
                    this, SLOT(_date_updated()));
            m_dateTimeEnd = new QDateTimeEdit;
            m_dateTimeEnd->setCalendarPopup(true);
            connect(m_dateTimeEnd, SIGNAL(dateTimeChanged(QDateTime)),
                    this, SLOT(_date_updated()));

            m_checkStart->setChecked(true);
            m_checkEnd->setChecked(true);

            connect(m_checkStart, SIGNAL(toggled(bool)), m_dateTimeStart, SLOT(setEnabled(bool)));
            connect(m_checkEnd, SIGNAL(toggled(bool)), m_dateTimeEnd, SLOT(setEnabled(bool)));
            connect(m_checkStart, SIGNAL(toggled(bool)), this, SLOT(_refresh_date_range()));
            connect(m_checkEnd, SIGNAL(toggled(bool)), this, SLOT(_refresh_date_range()));

            QSizePolicy sp;
            sp.setHorizontalPolicy(QSizePolicy::Expanding);
            sp.setHorizontalStretch(1);
            m_dateTimeStart->setSizePolicy(sp);
            m_dateTimeEnd->setSizePolicy(sp);
        }

        ui->gridLayout->addWidget(m_checkStart, 0, 2);
        ui->gridLayout->addWidget(m_dateTimeStart, 0, 3, 1, 2);
        ui->gridLayout->addWidget(m_checkEnd, 1, 2);
        ui->gridLayout->addWidget(m_dateTimeEnd, 1, 3, 1, 2);
        m_checkStart->show();
        m_checkEnd->show();
        m_dateTimeStart->show();
        m_dateTimeEnd->show();
        break;
    default:
        break;
    }

    _date_updated();
}

void SelectDateRange::_date_updated()
{
    bool already_suppressing = m_suppressUpdates;
    m_suppressUpdates = true;
    switch((ComboBoxItemEnum)ui->comboBox->currentIndex())
    {
    case Day:
        m_dateEdit->setDisplayFormat(m_dayFormat);
        break;
    case Week:
        m_dateEdit->setDisplayFormat(m_dayFormat);
        break;
    case Month:
        m_dateEdit->setDisplayFormat("MMM yyyy");
        break;
    case Year:
        m_dateEdit->setDisplayFormat("yyyy");
        break;
    case AllTime:
        break;
    case Custom:
    {
        Range<QDate> sel = GetDateRange();
        m_dateTimeStart->setDisplayFormat(m_dayFormat);
        m_dateTimeEnd->setDisplayFormat(m_dayFormat);

        m_checkStart->setChecked(sel.HasLowerBound());
        m_checkEnd->setChecked(sel.HasUpperBound());

        if(sel.HasLowerBound())
            m_dateTimeStart->setDate(sel.LowerBound().Value());
        if(sel.HasUpperBound())
            m_dateTimeEnd->setDate(sel.UpperBound().Value());
    }
        break;
    default:
        break;
    }
    m_suppressUpdates = already_suppressing;

    _refresh_date_range();
}

SelectDateRange::ComboBoxItemEnum SelectDateRange::GetComboBoxSelection() const
{
    return (ComboBoxItemEnum)ui->comboBox->currentIndex();
}

void SelectDateRange::SetComboBoxSelection(ComboBoxItemEnum c)
{
    ui->comboBox->setCurrentIndex((int)c);
}

QString SelectDateRange::GetDayFormat() const
{
    return m_dayFormat;
}

void SelectDateRange::SetDayFormat(const QString &d)
{
    m_dayFormat = d;
    _combobox_index_changed();
}


END_NAMESPACE_GUTIL1;
