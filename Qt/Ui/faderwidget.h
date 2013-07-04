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

#ifndef EFFECTSWIDGETS_H
#define EFFECTSWIDGETS_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include "gutil_macros.h"
#include <QWidget>
#include <QMutex>

namespace GUtil{ namespace QT{ namespace UI{


/** This class causes a QWidget to fade in/out.

    Use this code snippet as an example for how to use this class:

    FaderWidget *fader = new FaderWidget(<Widget that you want faded>);
    fader->fadeIn();
*/
class FaderWidget :
        public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QColor fadeColor READ fadeColor WRITE setFadeColor)
    Q_PROPERTY(int fadeDuration READ fadeDuration WRITE setFadeDuration)
public:

    FaderWidget(QWidget *par = 0, int fade_duration = 333, int start_delay = 0);

    /** The color it will fade to */
    inline QColor fadeColor() const{ return color; }
    /** The color it will fade to */
    inline void setFadeColor(const QColor &newColor){ color = newColor; }

    /** How many milliseconds it takes to complete a fade operation */
    inline int fadeDuration() const{ return duration; }
    /** How many milliseconds it takes to complete a fade operation */
    inline void setFadeDuration(int milliseconds){ duration = milliseconds; }


public slots:

    void fadeOut(bool skip_fade = false);
    void fadeIn(bool skip_fade = false);
    void toggleFade(bool skip_fade = false);


signals:

    void doneFading(bool faded_in);


protected:

    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *, QEvent *);


private slots:

    void start_fading();
    void _start();


private:
    QTimer *timer;
    int currentAlpha;
    QColor color;
    int duration;
    int delay;
    bool _fade_in;
    bool skipped_fade;
    QMutex thislock;
    QMutex fadelock;

};

}}}


#endif // GUI_FUNCTIONALITY

#endif // EFFECTSWIDGETS_H
