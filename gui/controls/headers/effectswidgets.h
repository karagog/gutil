#ifndef EFFECTSWIDGETS_H
#define EFFECTSWIDGETS_H

#include <QWidget>

namespace GUtil
{
    namespace QtControls
    {
        namespace EffectsWidgets
        {
            class FaderWidget : public QWidget
            {
                Q_OBJECT
                Q_PROPERTY(QColor fadeColor READ fadeColor \
                           WRITE setFadeColor)
                        Q_PROPERTY(int fadeDuration READ fadeDuration \
                                   WRITE setFadeDuration)
                Q_PROPERTY(bool willFadeIn READ willFadeIn WRITE setWillFadeIn)
            public:
                        FaderWidget(QWidget *par, bool fade_in,
                                    int fade_duration = 333, int start_delay = 0);

                QColor fadeColor() const;
                void setFadeColor(const QColor &newColor);
                int fadeDuration() const;
                void setFadeDuration(int milliseconds);
                bool willFadeIn();
                void setWillFadeIn(bool);

            public slots:
                void startFading();

                void fadeOut();
                void fadeIn();
                void toggleFade();

            signals:
                void doneFading(bool faded_in);

            protected:
                void paintEvent(QPaintEvent *event);
                bool eventFilter(QObject *, QEvent *);

            private slots:
                void _start();

            private:
                QTimer *timer;
                int currentAlpha;
                QColor color;
                int duration;
                int delay;
                bool _fade_in;

                // Use this code snippet as an example for how to use this class
                //QPointer<FaderWidget> faderwidget;
//                void QWidget::fadeInWidget(int index)
//                {
//                    if (faderWidget)
//                        faderWidget->close();
//                    faderWidget = new FaderWidget(
//                            widgetToBeFadedInto);
//                    faderWidget->start();

                // How it works:
                // We create a new FaderWidget with the widget to be revealed
                //  as its parent, and call start on the FaderWidget
//                }
            };
        }
    }
}

#endif // EFFECTSWIDGETS_H
