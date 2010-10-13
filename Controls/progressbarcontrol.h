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

#ifndef LABELLEDPROGRESSBAR_H
#define LABELLEDPROGRESSBAR_H

#include <QVariant>
#include <QWidget>

class QLabel;
class QPushButton;
class QProgressBar;

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

            QLabel *label();
            QProgressBar *progressBar();
            QPushButton *pushButton();

        private:
            QProgressBar *_progressBar;
            QLabel *_label;
            QPushButton *button;
        };
    }
}

#endif // LABELLEDPROGRESSBAR_H
