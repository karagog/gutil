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

#ifndef LABELLEDPROGRESSBAR_H
#define LABELLEDPROGRESSBAR_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

namespace GUtil{ namespace QT{ namespace Controls{


/** Implements a progress bar that you can push like a button, and has a
    label across the front.
*/
class ProgressBarControl :
        public QWidget
{
    Q_OBJECT
public:

    explicit ProgressBarControl(QWidget *parent = 0);

    /** Enables the progress bar to be clicked like a button */
    void setButtonEnabled(bool which = true);

    /** Access to the label */
    QLabel &Label(){ return _label; }
    /** Access to the label */
    const QLabel &Label() const{ return _label; }

    /** Access to the progress bar */
    QProgressBar &ProgressBar(){return _progressBar; }
    /** Access to the progress bar */
    const QProgressBar &ProgressBar() const{return _progressBar; }

    /** Access to the push button */
    QPushButton &PushButton(){ return _button; }
    /** Access to the push button */
    const QPushButton &PushButton() const{ return _button; }


private:

    QProgressBar _progressBar;
    QLabel _label;
    QPushButton _button;

};


}}}

#endif // GUI_FUNCTIONALITY

#endif // LABELLEDPROGRESSBAR_H
