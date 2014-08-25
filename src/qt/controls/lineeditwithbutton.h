/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_LINE_EDIT_WITH_BUTTON_H
#define GUTIL_LINE_EDIT_WITH_BUTTON_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include "gutil_faderwidget.h"
#include <QToolButton>
#include <QLineEdit>

namespace GUtil{ namespace QT{


/** Implements a simple line edit, with a button on the far right that fades
    in magically and can do whatever you want it to do.

    You can access the line edit and button directly via the member functions,
    and attach whatever signals/filters you want to their events.

    To show the button, call ShowButton() and you can control how fast it fades.
    The same thing goes for HideButton().
*/
class LineEditWithButton :
        public QWidget
{
    Q_OBJECT
    QToolButton m_button;
    QLineEdit m_lineEdit;
    FaderWidget m_fader;
public:

    /** Creates a LineEditWithButton with the given button action.
        You must supply an action to take when the button is clicked,
        otherwise why are you using the line edit with button?
    */
    explicit LineEditWithButton(QAction *button_action, QWidget *par = 0);

    /** Show the button if it's hidden.
        You can optionally override the default times used to fade.
    */
    void ShowButton(int fade_duration_ms = -1, int fade_delay = -1);

    /** If the button is visible, hide it.
        You can optionally override the default times used to fade.
    */
    void HideButton(int fade_duration_ms = -1, int fade_delay = -1);

    /** Returns true if the button is currently visible (or in the process of fading in/out). */
    bool IsButtonVisible() const{ return !Button().isHidden(); }


    /** Gives access to the button instance */
    QToolButton &Button(){ return m_button; }
    /** Gives access to the button instance */
    QToolButton const &Button() const{ return m_button; }

    /** Gives access to the lineedit instance */
    QLineEdit &LineEdit(){ return m_lineEdit; }
    /** Gives access to the lineedit instance */
    QLineEdit const &LineEdit() const{ return m_lineEdit; }

};


}}

#endif  // GUI_FUNCTIONALITY

#endif // GUTIL_LINE_EDIT_WITH_BUTTON_H
