/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "lineeditwithbutton.h"
#include "Ui/faderwidget.h"
#include <QHBoxLayout>
USING_NAMESPACE_GUTIL2(QT, UI);

NAMESPACE_GUTIL2(QT, Controls);


LineEditWithButton::LineEditWithButton(QWidget *par, bool btn_visible, int fade_duration) :
    QWidget(par)
{
    button.hide();

    button.setText("...");

    QSizePolicy p1;
    p1.setHorizontalPolicy(QSizePolicy::Expanding);
    p1.setVerticalPolicy(QSizePolicy::Expanding);
    p1.setHorizontalStretch(1);
    button.setSizePolicy(p1);
    button.setMaximumWidth(40);

    QSizePolicy p2;
    p2.setHorizontalPolicy(QSizePolicy::Expanding);
    p2.setVerticalPolicy(QSizePolicy::Expanding);
    p2.setHorizontalStretch(8);
    line_edit.setSizePolicy(p2);

    setLayout(new QHBoxLayout(this));
    layout()->setContentsMargins(0,0,0,0);
    layout()->setSpacing(0);
    layout()->addWidget(&line_edit);
    layout()->addWidget(&button);

    // Attach a fader to the button
    _fader = new FaderWidget(&button,
                             fade_duration == -1 ? 500 : fade_duration,
                             350);

    if(btn_visible)
        _fader->fadeIn();

    // Set up the focus policies so that my child widgets NEVER get focus
    line_edit.setFocusPolicy(Qt::NoFocus);
    button.setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);
}


END_NAMESPACE_GUTIL2;
