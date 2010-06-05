#include "effectswidgets.h"
#include <QPainter>
#include <QTimer>
#include <QEvent>
#include <QApplication>
using namespace GUtil::QtControls::EffectsWidgets;

FaderWidget::FaderWidget(QWidget *par, int fade_duration, int start_delay)
    : QWidget(par)
{
    if (par)
        color = par->palette().window().color();
    else
        color = Qt::white;

    _fade_in = false;

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

QColor FaderWidget::fadeColor() const
{
    return color;
}

void FaderWidget::setFadeColor(const QColor &newColor)
{
    color = newColor;
}

int FaderWidget::fadeDuration() const
{
    return duration;
}

void FaderWidget::setFadeDuration(int milliseconds)
{
    duration = milliseconds;
}

void FaderWidget::start_fading()
{
    if(timer->isActive())
    {
        // don't allow to fade while it's in the middle of fading
        return;
    }

    if(_fade_in)
        currentAlpha = 255;
    else
        currentAlpha = 0;

    show();

    // If they disabled the fade, then we ignore the delay
    if(duration == 0)
        _start();

    // Check if the parent is in the correct state before we fade it
    else if((_fade_in && parentWidget()->isHidden()) ||
       (!_fade_in && !parentWidget()->isHidden()))
    {
        QTimer::singleShot(delay, this, SLOT(_start()));
    }
}

void FaderWidget::_start()
{
    if(_fade_in)
        parentWidget()->showNormal();

    timer->start(40);
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

void FaderWidget::fadeIn()
{
    _fade_in = true;
    start_fading();
}

void FaderWidget::fadeOut(bool skip_fade)
{
    if(skip_fade)
        hide();
    else
    {
        _fade_in = false;
        start_fading();
    }
}

void FaderWidget::toggleFade()
{
    _fade_in = !_fade_in;
    start_fading();
}
