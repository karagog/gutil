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

#ifndef WIDGETHELPERS_H
#define WIDGETHELPERS_H

template <class T> class QList;
class QWidget;

namespace GUtil
{
    namespace Utils
    {
        class WidgetHelpers
        {
        public:

            // Center the second widget over the first one
            static void CenterOverWidget(QWidget *, QWidget *);

            // Put the widget in the center of the screen
            static void CenterInScreen(QWidget *);

            // Lines up the widgets and spaces them evenly
            static void AlignWidgets(const QList<QWidget *>&,
                                     bool horizontally = true);

        };
    }
}

#endif // WIDGETHELPERS_H
