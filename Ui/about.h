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

#ifndef GUTIL_ABOUT_H
#define GUTIL_ABOUT_H

#include <QDialog>
#include <QLabel>
#include <QPlainTextEdit>
#include "imageframe.h"

namespace GUtil{ namespace UI{


// A generic about window
class About :
        public QDialog
{
    Q_OBJECT
public:

    explicit About(QWidget *parent = 0, bool show_about_gutil = true);

    // You can manipulate these members directly to set the title and about text
    QLabel Title;
    QLabel BuildInfo;
    QPlainTextEdit Text;

    // The image will appear next to the title, and as the window icon.  It can be
    //  a regular filename or a resource name (:/resource.bmp for example)
    void SetImage(const QString &filename);

    // Add another about pushbutton
    void AddPushButton(QPushButton *);


private slots:

    void _about_gutil();


private:

    ImageFrame _imageFrame;

    QWidget *m_buttonWidget;
    QWidgetList m_buttonList;

};


}}

#endif // GUTIL_ABOUT_H
