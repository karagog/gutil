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

#include "timelineview.h"
#include <QPaintEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QApplication>
#include <QItemSelection>
#include <QRgb>

#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>

#include "gutil_macros.h"

#define MIN_WIDTH 500
#define TIMELINE_TOPBOTTOM_MARGIN 11
GUTIL_USING_NAMESPACE(ModelViews);


TimelineView::TimelineView(QWidget *p)
    :QAbstractItemView(p),
    _scale_factor(1),
    _origin_point(70, TIMELINE_TOPBOTTOM_MARGIN),
    _range_in_seconds(0),
    m_rubberBand(0),
    m_currentHighlighter(QRubberBand::Rectangle, this)
{

}


void TimelineView::paintEvent(QPaintEvent *ev)
{
    QAbstractItemView::paintEvent(ev);

    QPainter p(viewport());
    p.setRenderHint(QPainter::Antialiasing);

    // Translate the painter so it paints in the scroll area
    p.translate(viewport()->rect().x() - horizontalScrollBar()->value(),
                viewport()->rect().y() - verticalScrollBar()->value());

    // Draw the timeline
    QPoint finish_point( _finish_point() );
    p.drawLine(_origin_point, finish_point);

    p.drawLine(_origin_point, QPoint(_origin_point.x() + 10, _origin_point.y()));
    p.drawLine(finish_point, QPoint(finish_point.x() + 10, finish_point.y()));

    if(_start_time.isNull() ||
       _end_time.isNull() ||
       _end_time <= _start_time)
    {
        p.drawText(QRect(_origin_point.x(), _origin_point.y(), 500, _finish_point().y() - _origin_point.y()),
                   Qt::AlignVCenter, "Invalid Time Range Selection");
        p.end();
        return;
    }

    QDateTime tmptime( _start_time );
    int days(_start_time.daysTo(_end_time));
    int time_resolution(0);
    int time_increment_in_seconds(0);
    QString fmt;

    if(days == 1)
    {
        // Show 24 hours
        time_resolution = 24;
        time_increment_in_seconds = 60 * 60;

        fmt = "h:mm";
    }
    else if(days == 7)
    {
        // Show 7 days
        time_resolution = 7;
        time_increment_in_seconds = 60 * 60 * 24;

        fmt = "ddd";
    }
    else if(days >= 28 && days <= 31)
    {
        // Show all the days of the month
        time_resolution = days;
        time_increment_in_seconds = 60 * 60 * 24;

        fmt = "MMM d";
    }
    else if(days >= 365)
    {
        // Show 12 months
        time_resolution = 12;
        time_increment_in_seconds = -1;

        fmt = "MMM";
    }

    // Draw the tick marks
    {
        int m_y(-1);
        QDateTime m_time;
        for(int i = 0; i <= time_resolution; i++)
        {
            int time_til( _start_time.secsTo(tmptime) );
            int y( _origin_point.y() +
                  (long long)(finish_point.y() - _origin_point.y()) * time_til / _range_in_seconds );

            p.drawLine(_origin_point.x() - 5, y, _origin_point.x(), y);
            p.drawText(11, y - 10, 60, 20, Qt::AlignLeft | Qt::AlignTop,
                       tmptime.toString(fmt));

            // Advance the time
            if(time_increment_in_seconds == -1)
                tmptime = tmptime.addMonths(1);
            else
                tmptime = tmptime.addSecs(time_increment_in_seconds);


            if(m_y != -1)
            {
                // Analyze the gap between hash marks and see if there's room to squeeze another
                //  set of hashes in there
            }


            // Remember some values for the next time through the loop
            m_y = y;
            m_time = tmptime;
        }
    }


    // Draw the actual items in the model
    if(model())
    {
        for(int i = 0; i < model()->rowCount(); i++)
        {
            QModelIndex ind( model()->index(i, 0) );
            if(ind.data(StartDate).toDateTime() >= _end_time ||
               ind.data(EndDate).toDateTime() <= _start_time)
                continue;
            _draw_item(ind, p);
        }

        if(!currentIndex().isValid())
            m_currentHighlighter.hide();
    }
}


// The selected color is yellow
const QRgb SELECTED_RGB(qRgb(102, 153, 204));

void TimelineView::_draw_item(const QModelIndex &ind, QPainter &p)
{
    QRect item_rect( itemRect(ind) );

    QVariant color_variant = ind.data(Qt::BackgroundRole);
    QColor c;
    if(color_variant.isValid())
        c = color_variant.value<QColor>();
    else
        c = QColor(255, 255, 255);  // white

    if(selectionModel()->isSelected(ind))
        c = QColor(SELECTED_RGB);

    p.fillRect(item_rect, c);
    p.drawRect(item_rect);

    if(ind == currentIndex())
    {
        m_currentHighlighter.setGeometry(item_rect.translated(viewport()->pos()).translated(
                                             -horizontalScrollBar()->value(), -verticalScrollBar()->value()));
        m_currentHighlighter.show();
    }

    p.save();
    p.setFont(ind.data(Qt::FontRole).value<QFont>());
    p.drawText(item_rect.left() + 20, item_rect.top(), item_rect.width(), item_rect.height(),
               Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap,
               ind.data(Qt::DisplayRole).toString());
    p.restore();
}

QRect TimelineView::visualRect(const QModelIndex &index) const
{
    QRect rect( itemRect(index) );
    if(rect.isValid())
        return QRect(rect.left() - horizontalScrollBar()->value(),
                     rect.top() - verticalScrollBar()->value(),
                     rect.width(), rect.height());
    return rect;
}

QRect TimelineView::itemRect(const QModelIndex &index) const
{
    QDateTime start( index.data(StartDate).toDateTime() );
    QDateTime end( index.data(EndDate).toDateTime() );

    QPoint finish_point( _finish_point() );
    int starty(_origin_point.y() + (finish_point.y() - _origin_point.y()) *
               ( (double)_start_time.secsTo(start) / _range_in_seconds ));
    int endy(_origin_point.y() + (finish_point.y() - _origin_point.y()) *
             ( (double)_start_time.secsTo(end) / _range_in_seconds ));

    return QRect(_origin_point.x(), starty, 400, endy - starty);
}

void TimelineView::scrollTo(const QModelIndex &index, ScrollHint)
{
    QRect area(viewport()->rect());
    QRect rect(visualRect(index));

    if (rect.left() < area.left())
        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() + rect.left() - area.left());
    else if (rect.right() > area.right())
        horizontalScrollBar()->setValue(
            horizontalScrollBar()->value() + qMin(
                rect.right() - area.right(), rect.left() - area.left()));

    if (rect.top() < area.top())
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() + rect.top() - area.top());
    else if (rect.bottom() > area.bottom())
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() + qMin(
                rect.bottom() - area.bottom(), rect.top() - area.top()));
}

QModelIndex TimelineView::indexAt(const QPoint &point) const
{
    QModelIndex ret;
    if(model())
    {
        for(int i = 0; i < model()->rowCount(); i++)
        {
            QModelIndex tmp( model()->index(i, 0) );
            if(visualRect(tmp).contains(point))
            {
                ret = tmp;
                break;
            }
        }
    }
    visualRect(ret).contains(point);
    return ret;
}

QModelIndex TimelineView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    QModelIndex ret;
    QModelIndex cur( currentIndex() );

    switch(cursorAction)
    {
    case MoveDown:

        break;
    default:
        break;
    }

    return ret;
}

int TimelineView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int TimelineView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

bool TimelineView::isIndexHidden(const QModelIndex &index) const
{
    return false;
}

void TimelineView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
    QItemSelection sel;

    if(model())
    {
        for(int i = 0; i < model()->rowCount(); i++)
        {
            QModelIndex ind(model()->index(i, 0));
            if(itemRegion(ind).intersects(rect))
                sel.merge(QItemSelection(ind, ind), command);
        }
    }

    selectionModel()->select(sel, command);
}

QRegion TimelineView::visualRegionForSelection(const QItemSelection &selection) const
{
    QRegion ret;

    QModelIndexList il( selection.indexes() );
    for(int i = 0; i < il.count(); i++)
    {
        ret = ret.united(itemRegion(il[i]));
    }

    return ret;
}

QRegion TimelineView::itemRegion(const QModelIndex &ind) const
{
    return QRegion(visualRect(ind));
}


void TimelineView::_update_scrollbars()
{
    horizontalScrollBar()->setPageStep(viewport()->width());
    horizontalScrollBar()->setRange(0, qMax(0, MIN_WIDTH - viewport()->width()));
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setRange(
            0, qMax(0, (_finish_point().y() + TIMELINE_TOPBOTTOM_MARGIN) - viewport()->height()));
}


#define SCROLL_INCREMENT 0.005

void TimelineView::wheelEvent(QWheelEvent *ev)
{
    if(QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
    {
        double newval = _scale_factor + SCROLL_INCREMENT * ev->delta();
        if(newval > 0)
        {
            _scale_factor = newval;
            _update_scrollbars();
            viewport()->update();
        }
    }
    else
    {
        verticalScrollBar()->setSliderPosition(
                verticalScrollBar()->sliderPosition() - ev->delta() / 2);
    }
}

void TimelineView::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers().testFlag(Qt::ControlModifier))
    {
        if(event->key() == Qt::Key_0)
        {
            // Restore the default scale
            _scale_factor = 1;
            _update_scrollbars();
            viewport()->update();
        }
    }
}

void TimelineView::SetStartTime(const QDateTime &dt)
{
    _start_time = dt;
    _range_in_seconds = _start_time.secsTo(_end_time);
    viewport()->update();
}

void TimelineView::SetEndTime(const QDateTime &dt)
{
    _end_time = dt;
    _range_in_seconds = _start_time.secsTo(_end_time);
    viewport()->update();
}

void TimelineView::SetStartAndEndTime(const QDateTime &start, const QDateTime &end)
{
    _start_time = start;
    _end_time = end;
    _range_in_seconds = _start_time.secsTo(_end_time);
    viewport()->update();
}

void TimelineView::resizeEvent(QResizeEvent *event)
{
    QAbstractItemView::resizeEvent(event);

    _update_scrollbars();
}

void TimelineView::scrollContentsBy(int dx, int dy)
{
    viewport()->scroll(dx, dy);
}

void TimelineView::mousePressEvent(QMouseEvent *event)
{
    QAbstractItemView::mousePressEvent(event);

    m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    m_mouseFirstClick = event->pos();

    m_rubberBand->setGeometry(QRect(event->pos(), QSize()));
    m_rubberBand->show();
}

void TimelineView::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseReleaseEvent(event);

    if(!m_rubberBand.isNull())
        delete m_rubberBand.data();
}

void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseMoveEvent(event);

    if(!m_rubberBand.isNull())
        m_rubberBand->setGeometry(QRect(m_mouseFirstClick, event->pos()).normalized());
}

QPoint TimelineView::_finish_point() const
{
    return QPoint(_origin_point.x(), _scale_factor * 500);
}

void TimelineView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    viewport()->update();
}
