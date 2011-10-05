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

#include "gwidget.h"
#include "gutil.h"
#include "Core/exception.h"
#include <QApplication>
#include <QDesktopWidget>
GUTIL_USING_NAMESPACE(GUICustom);

GWidget::GWidget(QWidget *parent)
    :QWidget(parent)
{}

void GWidget::CenterOverWidget(QWidget *main, QWidget *child)
{
    QPoint p = main->pos();

    child->move(p.x() + ((main->width() - child->width()) / 2),
                p.y() + ((main->height() - child->height()) / 2));
}

void GWidget::CenterInScreen(QWidget *w)
{
    QRect r = QApplication::desktop()->availableGeometry();
    int x( r.width() / 2 - (w->size().width() / 2) );
    int y( r.height() / 2 - (w->size().height() / 2) );

    // In case the window is bigger than the screen, align the top of the window
    //  with the top of the screen, to allow the user access to the title bar.
    if(r.height() < w->height())
        y = r.top();

    w->move(x, y);
}

void GWidget::AlignWidgets(
        const QList<QWidget *>&wl,
        bool horizontally)
{
    THROW_NEW_GUTIL_EXCEPTION( GUtil::Core::NotImplementedException );
}

QWidget *GWidget::FindTopMostParent(QWidget *w)
{
    QWidget *p = w->parentWidget();
    if(!p)
        return w;

    return FindTopMostParent(p);
}
