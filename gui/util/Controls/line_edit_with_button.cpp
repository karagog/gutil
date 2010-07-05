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

#include "line_edit_with_button.h"
#include "effectswidgets.h"
#include <QPushButton>
#include <QLineEdit>
#include <QShowEvent>
#include <QEvent>
#include <QHBoxLayout>
using namespace GUtil::QtUtil::Controls;
using namespace GUtil::QtUtil::Controls::EffectsWidgets;

line_edit_with_button::line_edit_with_button(QWidget *par, bool btn_visible, int fade_duration) :
    QWidget(par)
{
    button = new QPushButton(this);
    line_edit = new QLineEdit(this);
    button->hide();

    button->setText("...");

    QSizePolicy p1;
    p1.setHorizontalPolicy(QSizePolicy::Expanding);
    p1.setVerticalPolicy(QSizePolicy::Expanding);
    p1.setHorizontalStretch(1);
    button->setSizePolicy(p1);
    button->setMaximumWidth(40);

    QSizePolicy p2;
    p2.setHorizontalPolicy(QSizePolicy::Expanding);
    p2.setVerticalPolicy(QSizePolicy::Expanding);
    p2.setHorizontalStretch(8);
    line_edit->setSizePolicy(p2);

    setLayout(new QHBoxLayout(this));
    layout()->setContentsMargins(0,0,0,0);
    layout()->setSpacing(0);
    layout()->addWidget(line_edit);
    layout()->addWidget(button);

    // Attach a fader to the button
    _fader = new FaderWidget(button,
                             fade_duration == -1 ? 500 : fade_duration,
                             350);

    if(btn_visible)
        _fader->fadeIn();

    // Set up the focus policies so that my child widgets NEVER get focus
    line_edit->setFocusPolicy(Qt::NoFocus);
    button->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);
}

line_edit_with_button::~line_edit_with_button()
{
}

void line_edit_with_button::showButton()
{
    if(button->isHidden())
        _fader->fadeIn();
}

void line_edit_with_button::hideButton()
{
    if(!button->isHidden())
        _fader->fadeIn();
}

void line_edit_with_button::toggleButton()
{
    _fader->toggleFade();
}

void line_edit_with_button::focusInEvent(QFocusEvent *e)
{
    line_edit->event(e);
}

void line_edit_with_button::keyPressEvent(QKeyEvent *e)
{
    line_edit->event(e);
}

void line_edit_with_button::keyReleaseEvent(QKeyEvent *e)
{
    line_edit->event(e);
}

QPushButton *line_edit_with_button::pushButton() const
{
    return button;
}

QLineEdit *line_edit_with_button::lineEdit() const
{
    return line_edit;
}

FaderWidget * line_edit_with_button::faderWidget() const
{
    return _fader;
}
