#include "effectswidgets.h"
#include <QPainter>
#include <QTimer>
#include <QEvent>
#include <QApplication>
using namespace GUtil::QtControls::EffectsWidgets;

FaderWidget::FaderWidget(QWidget *par, bool fade_in, int fade_duration, int start_delay)
    : QWidget(par)
{
    if (par)
        color = par->palette().window().color();
    else
        color = Qt::white;

    _fade_in = fade_in;

    duration = fade_duration;
    delay = start_delay;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(update()));

    setAttribute(Qt::WA_DeleteOnClose);

    // We want to intercept resize event so we can adjust our size
    par->installEventFilter(this);

    show();

    if(_fade_in)
    {
        currentAlpha = 255;
        ((QWidget *)parent())->hide();
    }
    else
    {
        currentAlpha = 0;
        ((QWidget *)parent())->show();
    }
}

bool FaderWidget::eventFilter(QObject *obj, QEvent *ev)
{
    bool ret = false;

    if(ev->type() == QEvent::Resize)
    {
        resize(((QWidget *)obj)->size());
        ret = true;
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

bool FaderWidget::willFadeIn()
{
    return _fade_in;
}

void FaderWidget::setWillFadeIn(bool val)
{
    _fade_in = val;
}

void FaderWidget::startFading()
{
    if(timer->isActive())
    {
        timer->stop();
    }

    if(_fade_in)
        currentAlpha = 255;
    else
        currentAlpha = 0;

    update();

    // Fade after a certain delay
    QTimer::singleShot(delay, this, SLOT(_start()));
}

void FaderWidget::_start()
{
    if(_fade_in)
        ((QWidget *)parent())->show();

    timer->start(30);
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
            currentAlpha -= (255 * timer->interval()) / duration;
            if (currentAlpha <= 0)
            {
                timer->stop();
                currentAlpha = 0;
            }
        }
        else
        {
            currentAlpha += (255 * timer->interval()) / duration;
            if (currentAlpha >= 255)
            {
                timer->stop();
                currentAlpha = 255;

                ((QWidget *)parent())->hide();
            }
        }
    }
}

void FaderWidget::fadeIn()
{
    ((QWidget *)parent())->hide();

    _fade_in = true;
    startFading();
}

void FaderWidget::fadeOut()
{
    _fade_in = false;
    startFading();
}
