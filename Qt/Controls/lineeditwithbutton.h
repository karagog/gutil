/*Copyright 2010-2012 George Karagoulis

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

#ifdef GUI_FUNCTIONALITY

#include "gutil_faderwidget.h"
#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QShowEvent>
#include <QEvent>

namespace GUtil{ namespace QT{ namespace Controls{


/** Implements a simple line edit, with a button on the far right that. */
class LineEditWithButton :
        public QWidget
{
    Q_OBJECT
public:

    /** Creates a LineEditWithButton with the given parameters*/
    explicit LineEditWithButton(QWidget *par = 0,
                                bool btn_visible = true,
                                int fade_duration = -1);

    /** Gives access to the button instance */
    QPushButton &PushButton(){ return button; }
    /** Gives access to the button instance */
    QPushButton const &PushButton() const{ return button; }

    /** Gives access to the lineedit instance */
    QLineEdit &LineEdit(){ return line_edit; }
    /** Gives access to the lineedit instance */
    QLineEdit const &LineEdit() const{ return line_edit; }

    /** Gives access to the fader widget instance */
    UI::FaderWidget *Fader(){ return _fader; }
    /** Gives access to the fader widget instance */
    UI::FaderWidget const *Fader() const{ return _fader; }


public slots:

    /** Show the button if it's hidden */
    void ShowButton(){ if(button.isHidden()) _fader->fadeIn(); }

    /** If the button is visible, hide it */
    void HideButton(){ if(!button.isHidden()) _fader->fadeIn(); }

    /** Toggles the button between visible and faded states */
    void ToggleButton(){ _fader->toggleFade(); }


protected:

    /** Overridden from QWidget */
    virtual void focusInEvent(QFocusEvent *e){ line_edit.event(e); }

    /** Overridden from QWidget */
    virtual void keyPressEvent(QKeyEvent *e){ line_edit.event(e); }

    /** Overridden from QWidget */
    virtual void keyReleaseEvent(QKeyEvent *e){ line_edit.event(e); }


private:

    QPushButton button;
    QLineEdit line_edit;
    UI::FaderWidget *_fader;

};


}}}

#endif  // GUI_FUNCTIONALITY

#endif // LINE_EDIT_WITH_BUTTON_H
