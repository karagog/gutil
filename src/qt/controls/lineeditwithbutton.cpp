/*Copyright 2010-2013 George Karagoulis

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
#include <QHBoxLayout>

NAMESPACE_GUTIL1(Qt);


LineEditWithButton::LineEditWithButton(QAction *a, QWidget *par)
    :QWidget(par),
      m_fader(&m_button, 500, 350)
{
    // The m_button starts out hidden, then fades in magically
    m_button.hide();
    m_button.setToolButtonStyle(::Qt::ToolButtonFollowStyle);
    m_button.setDefaultAction(a);
    m_button.setText("...");

    QSizePolicy p1;
    p1.setHorizontalPolicy(QSizePolicy::Expanding);
    p1.setVerticalPolicy(QSizePolicy::Expanding);
    p1.setHorizontalStretch(1);
    m_button.setSizePolicy(p1);
    m_button.setMaximumWidth(40);

    QSizePolicy p2;
    p2.setHorizontalPolicy(QSizePolicy::Expanding);
    p2.setVerticalPolicy(QSizePolicy::Expanding);
    p2.setHorizontalStretch(8);
    m_lineEdit.setSizePolicy(p2);

    setLayout(new QHBoxLayout(this));
    layout()->setContentsMargins(0,0,0,0);
    layout()->setSpacing(0);
    layout()->addWidget(&m_lineEdit);
    layout()->addWidget(&m_button);

    m_button.setFocusPolicy(::Qt::NoFocus);
    setFocusProxy(&m_lineEdit);
}

void LineEditWithButton::ShowButton(int fade_duration_ms, int fade_delay)
{
    if(!IsButtonVisible())
    {
        if(-1 != fade_delay)
            m_fader.SetFadeDelay(fade_delay);
        if(-1 != fade_duration_ms)
            m_fader.SetFadeDuration(fade_duration_ms);
        m_fader.FadeIn();
    }
}

void LineEditWithButton::HideButton(int fade_duration_ms, int fade_delay)
{
    if(IsButtonVisible())
    {
        if(-1 != fade_delay)
            m_fader.SetFadeDelay(fade_delay);
        if(-1 != fade_duration_ms)
            m_fader.SetFadeDuration(fade_duration_ms);
        m_fader.FadeOut();
    }
}


END_NAMESPACE_GUTIL1;
