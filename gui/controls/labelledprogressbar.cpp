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
    button = new QPushButton(this);
    button->setStyleSheet("background-color: rgba(0,0,0,0);");

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

    button->hide();
    progressBar->layout()->addWidget(button);
}

void LabelledProgressBar::setButtonEnabled(bool which)
{
    if(which)
    {
        button->show();
        ((QStackedLayout *)progressBar->layout())->setCurrentWidget(button);
    }
    else
    {
        button->hide();
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
