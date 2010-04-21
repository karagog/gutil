#include "effectswidgets.h"
#include <QPainter>
#include <QTimer>
#include <QEvent>
using namespace GUtil::QtControls::EffectsWidgets;

FaderWidget::FaderWidget(QWidget *parent, int fade_duration, int start_delay)
    : QWidget(parent)
{
    if (parent)
        color = parent->palette().window().color();
    else
        color = Qt::white;

    currentAlpha = 255;
    duration = fade_duration;
    delay = start_delay;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),
            this, SLOT(update()));

    setAttribute(Qt::WA_DeleteOnClose);

    // We want to intercept resize event so we can adjust our size
    parent->installEventFilter(this);
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

void FaderWidget::start()
{
    if(timer->isActive())
    {
        timer->stop();
        hide();
    }

    // Hide the widget
    currentAlpha = 255;
    show();

    // Fade in after a certain delay
    QTimer::singleShot(delay, this, SLOT(_start()));
}

void FaderWidget::_start()
{
    // Make sure we fit over top our parent, no matter what the size
    timer->start(50);
}

void FaderWidget::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    QColor semiTransparentColor = color;
    semiTransparentColor.setAlpha(currentAlpha);

    painter.fillRect(rect(), semiTransparentColor);

    if(timer->isActive())
    {
        currentAlpha -= (255 * timer->interval()) / duration;
        if (currentAlpha <= 0) {
            timer->stop();
            currentAlpha = 0;

            // Even though we're invisible we still hide so that we can show
            //  again later
            hide();
        }
    }
}
