/*Copyright 2010-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_WIDGET_H
#define GUTIL_WIDGET_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include <QWidget>

namespace GUtil{ namespace Qt{


/** My own subclass of QWidget with added functionality.

    It also contains useful static classes for manipulating QWidgets in general.
*/
class Widget :
        public QWidget
{
    Q_OBJECT
public:

    /** Creates a new Widget */
    explicit Widget(QWidget *parent = 0) :QWidget(parent){}


    /** Center the second widget over the first one
        \param keep_stationary Keep this widget where it is and center the other one over it.
        \param move_this_one Center this widget over the other one
        \sa CenterInScreen()
    */
    static void CenterOverWidget(QWidget *keep_stationary, QWidget *move_this_one);


    /** Put the widget in the center of the screen

        \sa CenterOverWidget()
    */
    static void CenterInScreen(QWidget *);


    /** Lines up the widgets and spaces them evenly across the screen.

        \param wl The list of widgets to aligna
        \param horizontally Determines whether to align the windows horizontally

        \todo Implement this function when you actually want to use it.
    */
    static void AlignWidgets(const QList<QWidget *>&,
                             bool horizontally = true);


    /** Returns the parent at the top of the QWidget hierarchy.

        \returns Returns the parent at the top of the QWidget hierarchy.
    */
    static QWidget *FindTopMostParent(QWidget *);

};


}}


#endif // GUI_FUNCTIONALITY

#endif // GUTIL_WIDGET_H
