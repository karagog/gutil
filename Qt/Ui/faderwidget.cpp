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

#include "faderwidget.h"
#include <QPainter>
#include <QTimer>
#include <QEvent>
#include <QApplication>

NAMESPACE_GUTIL2(QT, UI);


#define FADE_RESOLUTION 40

FaderWidget::FaderWidget(QWidget *par, int fade_duration, int start_delay)
    : QWidget(par)
{
    if (par)
        color = par->palette().window().color();
    else
        color = Qt::white;

    _fade_in = false;
    skipped_fade = false;

    duration = fade_duration;
    delay = start_delay;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    // We want to intercept resize event so we can adjust our size
    par->installEventFilter(this);
}

bool FaderWidget::eventFilter(QObject *obj, QEvent *ev)
{
    bool ret = false;

    if(obj == parent())
    {
        if(ev->type() == QEvent::Resize)
        {
            resize(parentWidget()->size());
            ret = true;
        }
    }

    return ret;
}

void FaderWidget::start_fading()
{
    if(!fadelock.tryLock())
        return;

    if(timer->isActive())
    {
        if(parentWidget()->isHidden())
            timer->stop();
        else
        {
            fadelock.unlock();
            return;
        }
    }

    if(_fade_in)
        currentAlpha = 255;
    else
        currentAlpha = 0;

    if(!skipped_fade)
        show();

    // If they disabled the fade, then we ignore the delay
    if(skipped_fade || duration == 0)
        _start();

    // Check if the parent is in the correct state before we fade it
    else if((_fade_in && parentWidget()->isHidden()) ||
       (!_fade_in && !parentWidget()->isHidden()))
    {
        QTimer::singleShot(delay, this, SLOT(_start()));
    }
    else
        fadelock.unlock();
}

void FaderWidget::_start()
{
    if(_fade_in)
        parentWidget()->showNormal();
    else if(skipped_fade)
        parentWidget()->hide();

    if(!skipped_fade)
        timer->start(FADE_RESOLUTION);

    fadelock.unlock();
}

void FaderWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    QColor semiTransparentColor = color;
    semiTransparentColor.setAlpha(currentAlpha);

    painter.fillRect(rect(), semiTransparentColor);

    if(timer->isActive())
    {
        if(_fade_in)
        {
            if(duration != 0)
            {
                currentAlpha -= (255 * timer->interval()) / duration;
            }

            if (duration == 0 || currentAlpha <= 0)
            {
                timer->stop();
                currentAlpha = 0;
                hide();

                emit doneFading(true);
            }
        }
        else
        {
            if(duration != 0)
            {
                currentAlpha += (255 * timer->interval()) / duration;
            }

            if (duration == 0 || currentAlpha >= 255)
            {
                timer->stop();
                currentAlpha = 255;

                // Hide the parent once we're faded out
                parentWidget()->hide();

                hide();

                emit doneFading(false);
            }
        }
    }
}

void FaderWidget::fadeIn(bool skip_fade)
{
    if(!thislock.tryLock())
        return;

    skipped_fade = skip_fade;
    _fade_in = true;
    start_fading();

    thislock.unlock();
}

void FaderWidget::fadeOut(bool skip_fade)
{
    if(!thislock.tryLock())
        return;

    skipped_fade = skip_fade;
    _fade_in = false;
    start_fading();

    thislock.unlock();
}

void FaderWidget::toggleFade(bool skip_fade)
{
    if(!thislock.tryLock())
        return;

    skipped_fade = skip_fade;
    _fade_in = !_fade_in;
    start_fading();

    thislock.unlock();
}


END_NAMESPACE_GUTIL2;
