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

#ifndef EFFECTSWIDGETS_H
#define EFFECTSWIDGETS_H

#include <QWidget>
#include <QMutex>

namespace GUtil
{
    namespace QtControls
    {
        namespace EffectsWidgets
        {
            // Use this code snippet as an example for how to use this class:

            // FaderWidget *fader = new FaderWidget(<Widget that you want faded>);
            // fader->fadeIn();


            class FaderWidget : public QWidget
            {
                Q_OBJECT
                Q_PROPERTY(QColor fadeColor READ fadeColor \
                           WRITE setFadeColor)
                        Q_PROPERTY(int fadeDuration READ fadeDuration \
                                   WRITE setFadeDuration)
            public:
                        FaderWidget(QWidget *par, int fade_duration = 333,
                                    int start_delay = 0);

                QColor fadeColor() const;
                void setFadeColor(const QColor &);
                int fadeDuration() const;
                void setFadeDuration(int);

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

            };
        }
    }
}

#endif // EFFECTSWIDGETS_H
