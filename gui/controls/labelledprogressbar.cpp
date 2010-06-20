/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "labelledprogressbar.h"
#include <QLabel>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProgressBar>

using namespace GUtil::QtControls;

LabelledProgressBar::LabelledProgressBar(QWidget *parent) :
    QWidget(parent)
{
    button = 0;
    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(false);

    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(progressBar);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    QFont f;  f.setBold(true);
    label->setFont(f);

    progressBar->setLayout(new QStackedLayout(progressBar));
    progressBar->layout()->setContentsMargins(0, 0, 0, 0);
    ((QStackedLayout*)progressBar->layout())->setStackingMode(QStackedLayout::StackAll);
    progressBar->layout()->addWidget(label);
}

void LabelledProgressBar::_button_clicked()
{
    emit clicked();
}

void LabelledProgressBar::useButton(bool which)
{
    if(which)
    {
        if(button == 0)
        {
            (button = new QPushButton(this))->setFlat(true);
            connect(button, SIGNAL(clicked()), this, SLOT(_button_clicked()));

            progressBar->layout()->addWidget(button);
            ((QStackedLayout *)progressBar->layout())->setCurrentWidget(button);
        }
    }
    else
    {
        if(button != 0)
        {
            delete button;
            button = 0;
        }
    }
}

QLabel *LabelledProgressBar::Label()
{
    return label;
}

QProgressBar *LabelledProgressBar::ProgressBar()
{
    return progressBar;
}

QPushButton *LabelledProgressBar::Button()
{
    return button;
}

void LabelledProgressBar::setValue(int val)
{
    progressBar->setValue(val);
}

int LabelledProgressBar::value()
{
    return progressBar->value();
}
