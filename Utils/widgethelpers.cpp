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

#ifdef GUI_FUNCTIONALITY

#include "widgethelpers.h"
#include "Core/exception.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QWidget>
#include <QList>
using namespace GUtil::Utils;

void WidgetHelpers::CenterOverWidget(QWidget *main, QWidget *child)
{
    QPoint p = main->pos();

    child->move(p.x() + ((main->width() - child->width()) / 2),
                p.y() + ((main->height() - child->height()) / 2));
}

void WidgetHelpers::CenterInScreen(QWidget *w)
{
    QRect r = QApplication::desktop()->availableGeometry();
    w->move(r.width() / 2 - (w->size().width() / 2),
            r.height() / 2 - (w->size().height() / 2));
}

void WidgetHelpers::AlignWidgets(
        const QList<QWidget *>&wl,
        bool horizontally)
{
    THROW_NEW_GUTIL_EXCEPTION2( Core::NotImplementedException, "" );
}

QWidget *WidgetHelpers::FindTopMostParent(QWidget *w)
{
    QWidget *p = w->parentWidget();
    if(!p)
        return w;

    return FindTopMostParent(p);
}

#endif // GUI_FUNCTIONALITY
