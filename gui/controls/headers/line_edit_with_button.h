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
            line_edit_with_button(QWidget *parent = 0);
            ~line_edit_with_button();

            QString text() const;

            void setButtonToolTip(const QString&);
            void setLineEditToolTip(const QString &);

        signals:
            void buttonClicked();
            void textChanged(QString);

        public slots:
            void setText(const QString&);
            void selectAll() const;

            void toggleButton();

        protected:
            void changeEvent(QEvent *);

            // These events are passed to the lineEdit itself
            void focusInEvent(QFocusEvent *);
            void focusOutEvent(QFocusEvent *);
            void keyPressEvent(QKeyEvent *);
            void keyReleaseEvent(QKeyEvent *);
            bool eventFilter(QObject *, QEvent *);

        private:
            Ui::line_edit_with_button *ui;

            QPointer<EffectsWidgets::FaderWidget> fader;

        private slots:
            void _btn_clicked();
            void _txt_changed(QString);
        };
    }
}

#endif // LINE_EDIT_WITH_BUTTON_H
