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

#include "widgethelpers.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>


using namespace GUtil::QtUtil::Tools;

void WidgetHelpers::centerOverWidget(QWidget *main, QWidget *child)
{
    QPoint p = main->pos();

    QPoint myCenter(main->rect().width() / 2, main->rect().height() / 2);
    QPoint hisCenter(child->rect().width() / 2, child->rect().height() / 2);

    child->move(p.x() + (myCenter.x() - hisCenter.x()),
                 p.y() + (myCenter.y() - hisCenter.y()));
}

void WidgetHelpers::centerInScreen(QWidget *w)
{
    QRect r = QApplication::desktop()->availableGeometry();
    w->move(r.width() / 2 - (w->size().width() / 2),
            r.height() / 2 - (w->size().height() / 2));
}
