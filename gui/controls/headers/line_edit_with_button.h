#ifndef LINE_EDIT_WITH_BUTTON_H
#define LINE_EDIT_WITH_BUTTON_H

#include <QWidget>
#include <QPointer>

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
            line_edit_with_button(QWidget *parent = 0, bool btn_visible = true);
            ~line_edit_with_button();

            QString text() const;

            QPushButton *pushButton() const;
            QLineEdit *lineEdit() const;

        public slots:
            void toggleButton();

        protected:
            // These events are passed to the lineEdit itself
            void focusInEvent(QFocusEvent *);
            void focusOutEvent(QFocusEvent *);
            void keyPressEvent(QKeyEvent *);
            void keyReleaseEvent(QKeyEvent *);
            bool eventFilter(QObject *, QEvent *);

            void changeEvent(QEvent *);

        private:
            Ui::line_edit_with_button *ui;

            QPointer<EffectsWidgets::FaderWidget> fader;
        };
    }
}

#endif // LINE_EDIT_WITH_BUTTON_H
