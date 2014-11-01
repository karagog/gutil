/*Copyright 2014-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "faderwidget.h"
#include <gutil/globals.h>
#include <QPainter>
#include <QEvent>
#include <QTimer>
USING_NAMESPACE_GUTIL;

#define FADE_RESOLUTION 40

NAMESPACE_GUTIL1(Qt);


FaderWidget::FaderWidget(QWidget *par, GUINT32 fade_duration, GUINT32 fade_delay)
    :QWidget(par),
      m_timerId(-1),
      m_alpha(0),
      m_delayCtr(0),
      m_fadeIn(false),
      _p_FadeColor(par->palette().window().color()),
      _p_FadeDuration(fade_duration),
      _p_FadeDelay(fade_delay)
{
    par->installEventFilter(this);
}

bool FaderWidget::eventFilter(QObject *obj, QEvent *ev)
{
    bool ret = false;
    if(obj == parent()){
        if(ev->type() == QEvent::Resize){
            resize(parentWidget()->size());
        }
    }
    return ret;
}

void FaderWidget::_fade_requested(bool skip_fade)
{
    if(skip_fade)
    {
        if(m_timerId != -1){
            killTimer(m_timerId);
            m_timerId = -1;
        }

        if(m_fadeIn){
            hide();
            parentWidget()->show();
        }
        else
            parentWidget()->hide();
    }
    else
    {
        m_delayCtr = GetFadeDelay();
        if(m_fadeIn)m_alpha = 255;
        else        m_alpha = 0;
        show();
        if(m_timerId == -1)
            m_timerId = startTimer(FADE_RESOLUTION);
    }
}

void FaderWidget::paintEvent(QPaintEvent *)
{
    QColor c = GetFadeColor();
    c.setAlpha(m_alpha);
    QPainter(this).fillRect(rect(), c);
}

void FaderWidget::timerEvent(QTimerEvent *ev)
{
    if(m_delayCtr > 0){
        m_delayCtr = Max(m_delayCtr - FADE_RESOLUTION, 0);
        return;
    }
    else if(m_delayCtr == 0){
        m_delayCtr = -1;

        // Show my parent after the delay is up
        if(m_fadeIn)
            parentWidget()->show();
    }

    const int diff = (float)(255 * FADE_RESOLUTION) / GetFadeDuration();
    if(m_fadeIn){
        m_alpha = Max(m_alpha - diff, 0);
        if(m_alpha == 0){
            // Done fading in, hide myself
            hide();
            killTimer(ev->timerId());
            emit DoneFading(true);
        }
    }
    else{
        m_alpha = Min(m_alpha + diff, 255);
        if(m_alpha == 255){
            // Done fading out, hide my parent
            parentWidget()->hide();
            killTimer(ev->timerId());
            emit DoneFading(false);
        }
    }

    // Update triggers my paint event
    update();
    ev->accept();
}

void FaderWidget::FadeIn(bool skip_fade)
{
    m_fadeIn = true;
    _fade_requested(skip_fade);
}

void FaderWidget::FadeOut(bool skip_fade)
{
    m_fadeIn = false;
    _fade_requested(skip_fade);
}

void FaderWidget::ToggleFade(bool skip_fade)
{
    m_fadeIn = !m_fadeIn;
    _fade_requested(skip_fade);
}


END_NAMESPACE_GUTIL1;
