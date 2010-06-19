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

#include <QProgressBar>
#include <QVariant>

class QLabel;
class QPushButton;
class QStackedLayout;

namespace GUtil
{
    namespace QtControls
    {
        class LabelledProgressBar : public QProgressBar
        {
            Q_OBJECT

        public:
            explicit LabelledProgressBar(QWidget *parent = 0);

            void useButton(bool which = true);

            QLabel *Label();
            QProgressBar *ProgressBar();
            QPushButton *Button();

            // Use this if you want to store any kind of data/state within the control
            QVariant data;

        signals:
            void clicked();

        private slots:
            void _button_clicked();

        private:
            QProgressBar *progressBar;
            QLabel *label;
            QPushButton *button;

            QStackedLayout *layout;
        };
    }
}


#endif // LABELLEDPROGRESSBAR_H
