/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_FADERWIDGET_H
#define GUTIL_FADERWIDGET_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include <gutil/macros.h>
#include <QWidget>

namespace GUtil{ namespace Qt{


/** This class causes a QWidget to fade in/out.

    Use this code snippet as an example for how to use this class:

    FaderWidget *fader = new FaderWidget(<Widget that you want faded>);
    fader->fadeIn();
*/
class FaderWidget :
        public QWidget
{
    Q_OBJECT

    int m_timerId;
    int m_alpha;
    int m_delayCtr;
    bool m_fadeIn;
public:

    /** You must give the fader a parent widget that will be faded in/out.
        You can use this constructor to conveniently set the fade options as well.
    */
    FaderWidget(QWidget *widget_to_be_faded, GUINT32 fade_duration = 333, GUINT32 fade_delay = 0);

    /** The color it will fade to */
    PROPERTY(FadeColor, QColor);

    /** The duration of fading, in milliseconds. */
    PROPERTY(FadeDuration, GUINT32);

    /** The delay before starting to fade, in milliseconds. */
    PROPERTY(FadeDelay, GUINT32);


public slots:

    /** Causes the widget to fade in. */
    void FadeIn(bool skip_fade = false);

    /** Causes the widget to fade out. */
    void FadeOut(bool skip_fade = false);

    /** Causes the widget to fade to a different state;
        if it was visible it will fade out, and vice versa.
    */
    void ToggleFade(bool skip_fade = false);


signals:

    /** Emitted when the fading has finished. */
    void DoneFading(bool faded_in);


protected:

    virtual void paintEvent(QPaintEvent *);
    virtual void timerEvent(QTimerEvent *);
    virtual bool eventFilter(QObject *, QEvent *);


private:

    void _fade_requested(bool skip_fade);

};

}}


#endif // GUI_FUNCTIONALITY

#endif // GUTIL_FADERWIDGET_H
