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

#include "progressbarcontrol.h"
#include <Qlabel>
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProgressBar>
using namespace GUtil;

Controls::ProgressBarControl::ProgressBarControl(QWidget *parent) :
    QWidget(parent)
{
    button = new QPushButton(this);
    button->setStyleSheet("background-color: rgba(0,0,0,0);");

    _progressBar = new QProgressBar(this);
    _progressBar->setTextVisible(false);

    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(_progressBar);

    _label = new QLabel(this);
    _label->setAlignment(Qt::AlignCenter);
    QFont f;  f.setBold(true);
    _label->setFont(f);

    _progressBar->setLayout(new QStackedLayout(_progressBar));
    _progressBar->layout()->setContentsMargins(0, 0, 0, 0);
    ((QStackedLayout*)_progressBar->layout())->setStackingMode(QStackedLayout::StackAll);
    _progressBar->layout()->addWidget(_label);

    button->hide();
    _progressBar->layout()->addWidget(button);
}

void Controls::ProgressBarControl::setButtonEnabled(bool which)
{
    if(which)
    {
        button->show();
        ((QStackedLayout *)_progressBar->layout())->setCurrentWidget(button);
    }
    else
    {
        button->hide();
    }
}

QLabel *Controls::ProgressBarControl::label()
{
    return _label;
}

QProgressBar *Controls::ProgressBarControl::progressBar()
{
    return _progressBar;
}

QPushButton *Controls::ProgressBarControl::pushButton()
{
    return button;
}