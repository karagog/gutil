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

#ifndef GUTIL_ABOUT_H
#define GUTIL_ABOUT_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include <QDialog>
#include <QLabel>
#include <QTextEdit>

namespace GUtil{ namespace QT{ namespace UI{


/** A generic about window that you can use to quickly set your own */
class About :
        public QDialog
{
    Q_OBJECT
public:

    explicit About(QWidget *parent = 0, bool show_about_gutil = true);

    /** Controls the title of the window */
    QLabel Title;

    /** Gives information about the version and/or date */
    QLabel BuildInfo;

    /** Controls the text content of the window */
    QTextEdit Text;

    /** Sets an image to display in the window
        \param path_to_resource Can be a resource or a file path
    */
    void SetImage(const QString &path_to_resource);

    /** Adds your own custom pushbutton to the interface.
        This extends the window's functionality, because the button
        can do literally anything.
    */
    void AddPushButton(QPushButton *);


private:

    QWidget _imageFrame;

    QWidget *m_buttonWidget;
    QWidgetList m_buttonList;

};


}}}

#endif // GUI_FUNCTIONALITY

#endif // GUTIL_ABOUT_H
