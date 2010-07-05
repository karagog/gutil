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
    namespace QtUtil
    {
        namespace Controls
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

            private:
                QPushButton *button;
                QLineEdit *line_edit;

                EffectsWidgets::FaderWidget *_fader;
            };
        }
    }
}

#endif // LINE_EDIT_WITH_BUTTON_H
