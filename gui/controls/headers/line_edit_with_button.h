#ifndef LINE_EDIT_WITH_BUTTON_H
#define LINE_EDIT_WITH_BUTTON_H

#include <QWidget>

namespace Ui {
    class line_edit_with_button;
}

class QShowEvent;
class QResizeEvent;
class QFocusEvent;
class QEvent;
class QPushButton;
class QLineEdit;

namespace GUtil
{
    namespace QtControls
    {
        namespace EffectsWidgets
        {
            class FaderWidget;
        }

        class line_edit_with_button : public QWidget {
            Q_OBJECT
        public:
            line_edit_with_button(QWidget *par = 0, bool btn_visible = true,
                                  int fade_duration = -1);
            ~line_edit_with_button();

            QPushButton *pushButton() const;
            QLineEdit *lineEdit() const;
            EffectsWidgets::FaderWidget *faderWidget() const;

        public slots:
            void showButton();
            void hideButton();
            void toggleButton();

        protected:
            // These events are passed to the lineEdit itself
            void focusInEvent(QFocusEvent *);
            void keyPressEvent(QKeyEvent *);
            void keyReleaseEvent(QKeyEvent *);

            void changeEvent(QEvent *);

        private:
            Ui::line_edit_with_button *ui;

            EffectsWidgets::FaderWidget *_fader;
        };
    }
}

#endif // LINE_EDIT_WITH_BUTTON_H
