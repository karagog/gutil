#ifndef LINE_EDIT_WITH_BUTTON_H
#define LINE_EDIT_WITH_BUTTON_H

#include "effectswidgets.h"
#include <QWidget>
#include <QPointer>

namespace Ui {
    class line_edit_with_button;
}

class QShowEvent;
class QResizeEvent;
class QFocusEvent;

namespace GUtil
{
    namespace QtControls
    {
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

        protected:
            void changeEvent(QEvent *);
            void focusInEvent(QFocusEvent *);

        private:
            Ui::line_edit_with_button *ui;

            QPointer<GUtil::QtControls::EffectsWidgets::FaderWidget> fader;

        private slots:
            void _btn_clicked();
            void _txt_changed(QString);
        };
    }
}

#endif // LINE_EDIT_WITH_BUTTON_H
