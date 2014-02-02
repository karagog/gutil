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
#include <QDateTimeEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QFormLayout>
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL2(QT, Controls);


SelectDateRange::SelectDateRange(QWidget *parent)
    :QWidget(parent),
      ui(new Ui::SelectDateRange),
      m_checkStart(NULL),
      m_checkEnd(NULL),
      m_dateTimeStart(NULL),
      m_dateTimeEnd(NULL)
{
    ui->setupUi(this);

    (new QVBoxLayout(ui->widget))
            ->setContentsMargins(0, 0, 0, 0);

    _combobox_index_changed(ui->comboBox->currentIndex());
}

SelectDateRange::~SelectDateRange()
{}

Range<QDateTime> SelectDateRange::GetDateRange() const
{
    Range<QDateTime> r;
    if(m_customWidget && m_customWidget->isVisible()){
        // Grab the range from the custom widget
        if(m_checkStart->isChecked()){
            r.SetLowerBound(m_dateTimeStart->dateTime());
        }
        if(m_checkEnd->isChecked()){
            r.SetUpperBound(m_dateTimeEnd->dateTime());
        }
    }
    else{

    }
    return r;
}

void SelectDateRange::SetDateRange(const Range<QDateTime> &r)
{

}

void SelectDateRange::_combobox_index_changed(int ind)
{
    // Grab the selection so we can restore it after rearranging the layout
    Range<QDateTime> sel = GetDateRange();

    // First update the control layout:
    switch((ComboBoxItemEnum)ind)
    {
    case Day:
    case Week:
    case Month:
    case Year:
    case AllTime:
        if(m_customWidget){
            ui->widget->layout()->removeWidget(m_customWidget);
            m_customWidget->hide();
        }

        if(!m_dateEdit){
            m_dateEdit = new QDateEdit;
            m_dateEdit->setCalendarPopup(true);
            //m_dateEdit->setAlignment(Qt::AlignRight);
        }

        ui->widget->layout()->addWidget(m_dateEdit);
        ui->horizontalLayout->setAlignment(ui->label, Qt::AlignLeft | Qt::AlignHCenter);
        ui->horizontalLayout->setAlignment(ui->comboBox, Qt::AlignLeft | Qt::AlignHCenter);

        m_dateEdit->setEnabled(AllTime != ind);
        m_dateEdit->show();

        break;
    case Custom:
        if(m_dateEdit){
            ui->widget->layout()->removeWidget(m_dateEdit);
            m_dateEdit->hide();
        }

        if(!m_customWidget)
        {
            // Initialize the custom widget
            m_customWidget = new QWidget;
            QFormLayout *fl = new QFormLayout(m_customWidget);
            fl->setContentsMargins(0,0,0,0);

            m_checkStart = new QCheckBox(tr("Date Start"), m_customWidget);
            m_checkStart->setChecked(sel.HasLowerBound());
            m_checkEnd = new QCheckBox(tr("Date End"), m_customWidget);
            m_checkEnd->setChecked(sel.HasUpperBound());
            m_dateTimeStart = new QDateTimeEdit(m_customWidget);
            m_dateTimeStart->setCalendarPopup(true);
            m_dateTimeStart->setDateTime(sel.LowerBound().Value());
            m_dateTimeEnd = new QDateTimeEdit(m_customWidget);
            m_dateTimeEnd->setCalendarPopup(true);
            m_dateTimeEnd->setDateTime(sel.UpperBound().Value());

            connect(m_checkStart, SIGNAL(toggled(bool)), m_dateTimeStart, SLOT(setEnabled(bool)));
            connect(m_checkEnd, SIGNAL(toggled(bool)), m_dateTimeEnd, SLOT(setEnabled(bool)));

            m_checkStart->setChecked(true);
            m_checkEnd->setChecked(true);

            fl->addRow(m_checkStart, m_dateTimeStart);
            fl->addRow(m_checkEnd, m_dateTimeEnd);
        }

        ui->widget->layout()->addWidget(m_customWidget);
        ui->horizontalLayout->setAlignment(ui->label, Qt::AlignLeft | Qt::AlignTop);
        ui->horizontalLayout->setAlignment(ui->comboBox, Qt::AlignLeft | Qt::AlignTop);

        m_customWidget->show();
        break;
    default:
        break;
    }


    // Then update the widgets
    switch((ComboBoxItemEnum)ind)
    {
    case Day:
        m_dateEdit->setDisplayFormat("MMM dd, yyyy");
        break;
    case Week:
        m_dateEdit->setDisplayFormat("MMM dd, yyyy");
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
        m_dateTimeStart->setDisplayFormat("MMM dd, yyyy");
        m_dateTimeEnd->setDisplayFormat("MMM dd, yyyy");
        break;
    default:
        break;
    }

//    dt_timeSelect.setDisplayFormat("MMM dd, yyyy");
//    dt_timeSelect.setEnabled(true);
}

void SelectDateRange::SetComboBoxSelection(ComboBoxItemEnum c)
{
    ui->comboBox->setCurrentIndex((int)c);
}


END_NAMESPACE_GUTIL2;
