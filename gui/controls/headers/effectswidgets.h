#ifndef EFFECTSWIDGETS_H
#define EFFECTSWIDGETS_H

#include <QWidget>

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
                void fadeIn();
                void toggleFade();

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
            };
        }
    }
}

#endif // EFFECTSWIDGETS_H
