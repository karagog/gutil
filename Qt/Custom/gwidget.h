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

#ifndef GWIDGET_H
#define GWIDGET_H

#ifdef GUI_FUNCTIONALITY

#include <QWidget>

namespace GUtil{ namespace QT{ namespace Custom{


/** My own subclass of QWidget with added functionality.

    It also contains useful static classes for manipulating QWidgets in general.
*/
class GWidget :
        public QWidget
{
    Q_OBJECT
public:

    /** Creates a new GWidget */
    inline explicit GWidget(QWidget *parent = 0) :QWidget(parent){}


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

        \todo Implement this function when you actually want to use it.
    */
    static void AlignWidgets(const QList<QWidget *>&,
                             bool horizontally = true);


    /** Returns the parent at the top of the QWidget hierarchy.

        \returns Returns the parent at the top of the QWidget hierarchy.
    */
    static QWidget *FindTopMostParent(QWidget *);

};


}}}


#endif // GUI_FUNCTIONALITY

#endif // GWIDGET_H
