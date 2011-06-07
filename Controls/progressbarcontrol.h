/*Copyright 2011 George Karagoulis

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

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

namespace GUtil
{
    namespace Controls
    {
        class ProgressBarControl : public QWidget
        {
            Q_OBJECT
        public:

            explicit ProgressBarControl(QWidget *parent = 0);

            void setButtonEnabled(bool which = true);

            inline QLabel &Label(){ return _label; }
            inline const QLabel &Label() const{ return _label; }

            inline QProgressBar &ProgressBar(){return _progressBar; }
            inline const QProgressBar &ProgressBar() const{return _progressBar; }

            inline QPushButton &PushButton(){ return _button; }
            inline const QPushButton &PushButton() const{ return _button; }


        private:

            QProgressBar _progressBar;
            QLabel _label;
            QPushButton _button;

        };
    }
}

#endif // LABELLEDPROGRESSBAR_H
