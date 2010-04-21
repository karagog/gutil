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
            public:
                        FaderWidget(QWidget *parent, int fade_duration = 333,
                                    int start_delay = 0);

                QColor fadeColor() const;
                void setFadeColor(const QColor &newColor);
                int fadeDuration() const;
                void setFadeDuration(int milliseconds);

            public slots:
                // Start the fade effect
                void start();

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
