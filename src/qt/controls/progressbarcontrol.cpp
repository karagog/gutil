/*Copyright 2010-2015 George Karagoulis

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
#include <gutil/macros.h>
#include <QStackedLayout>
#include <QVBoxLayout>

NAMESPACE_GUTIL1(Qt);


ProgressBarControl::ProgressBarControl(bool be, QWidget *parent)
    :QWidget(parent),
      _p_AutoShow(true),
      m_isCancellable(true)
{
    _button.setStyleSheet("background-color: rgba(0,0,0,0);");

    _progressBar.setTextVisible(false);

    setLayout(new QVBoxLayout(this));
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->addWidget(&_progressBar);

    _label.setAlignment(::Qt::AlignCenter);
    QFont f;  f.setBold(true);
    _label.setFont(f);

    _progressBar.setLayout(new QStackedLayout(&_progressBar));
    _progressBar.layout()->setContentsMargins(0, 0, 0, 0);
    ((QStackedLayout*)_progressBar.layout())->setStackingMode(QStackedLayout::StackAll);
    _progressBar.layout()->addWidget(&_label);

    _progressBar.layout()->addWidget(&_button);

    connect(&_button, SIGNAL(clicked()), this, SIGNAL(Clicked()));

    setButtonEnabled(be);

    if(GetAutoShow())
        hide();
}

void ProgressBarControl::SetIsCancellable(bool c)
{
    m_isCancellable = c;
    _button.setVisible(c);
}

void ProgressBarControl::setButtonEnabled(bool which)
{
    if(which)
    {
        _button.show();
        ((QStackedLayout *)_progressBar.layout())->setCurrentWidget(&_button);
    }
    else
    {
        _button.hide();
    }
}

void ProgressBarControl::SetProgress(int p, const QString &s)
{
    if(GetAutoShow())
    {
        if(p == _progressBar.maximum() && isVisible())
            hide();
        else if(p < _progressBar.maximum() && !isVisible())
            show();
    }

    _progressBar.setValue(p);
    if(_button.isVisible())
        _label.setText(QString(tr("%1%2"))
                       .arg(s)
                       .arg(m_isCancellable ? tr(" (Click to cancel)") : QString::null));
    else
        _label.setText(s);
}


END_NAMESPACE_GUTIL1;
