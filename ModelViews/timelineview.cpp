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
      _finish_point(_origin_point.x(), _origin_point.y()),
      _range_in_seconds(0),
      _resolution_in_seconds(1),
      m_rubberBand(0),
      m_currentHighlighter(QRubberBand::Rectangle, this),
      m_drag_cursor_offset(-1)
{
    setMouseTracking(true);

    _update_finish_point();
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
    p.drawLine(_origin_point, _finish_point);

    p.drawLine(_origin_point, QPoint(_origin_point.x() + 10, _origin_point.y()));
    p.drawLine(_finish_point, QPoint(_finish_point.x() + 10, _finish_point.y()));

    if(_start_time.isNull() ||
       _end_time.isNull() ||
       _end_time <= _start_time)
    {
        p.drawText(QRect(_origin_point.x(), _origin_point.y(), 500, _finish_point.y() - _origin_point.y()),
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
                  (long long)(_finish_point.y() - _origin_point.y()) * time_til / _range_in_seconds );

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


// The selected color
#define SELECTED_RGB 0xFF6699CC

void TimelineView::_draw_item(const QModelIndex &ind, QPainter &p)
{
    // The painter is being translated already to compensate for scrollbars, so
    //  we can use the real item rect without shifting it
    QRect item_rect( itemRect(ind) );

    // If they're in the middle of a drag/drop then adjust the rect
    if(m_draggingIndex.isValid() &&
            m_draggingIndex == ind)
    {
        _adjust_rect_for_dragDrop(item_rect);

        // TODO: Print the start/end times as they drag
        if(!m_drag_startDate.isNull())
        {

        }
        if(!m_drag_endDate.isNull())
        {

        }
    }

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
        rect.translate(-horizontalScrollBar()->value(),
                       -verticalScrollBar()->value());
    return rect;
}

QRect TimelineView::itemRect(const QModelIndex &index) const
{
    if(!index.isValid())
        return QRect();

    QDateTime start( index.data(StartDate).toDateTime() );
    QDateTime end( index.data(EndDate).toDateTime() );

    QPoint item_end( DateTimeToPoint(end) );
    item_end.setX(400);
    return QRect(DateTimeToPoint(start), item_end);
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
            0, qMax(0, (_finish_point.y() + TIMELINE_TOPBOTTOM_MARGIN) - viewport()->height()));
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
            _update_finish_point();

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
            _update_finish_point();

            _update_scrollbars();
            viewport()->update();
        }
    }
}

void TimelineView::SetStartTime(const QDateTime &dt)
{
    SetStartAndEndTime(dt, _end_time);
}

void TimelineView::SetEndTime(const QDateTime &dt)
{
    SetStartAndEndTime(_start_time, dt);
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


#define ADJUSTMENT_BUFFER 10

void TimelineView::mousePressEvent(QMouseEvent *event)
{
    QAbstractItemView::mousePressEvent(event);

    if(event->button() == Qt::LeftButton)
    {
        QPoint pos( event->pos() );
        QModelIndex ind( indexAt(pos) );
        if(ind.isValid())
        {
            // If they clicked on a valid index, then we let them drag it around the timeline,
            //  or adjust the start/end date
            QRect rect( visualRect(ind) );
            int diffTop( gAbs(pos.y() - rect.top()) );
            int diffBottom( gAbs(pos.y() - rect.bottom()) );

            m_draggingIndex = ind;

            if(diffTop <= ADJUSTMENT_BUFFER ||
                    diffBottom <= ADJUSTMENT_BUFFER)
            {
                // If they clicked near the top/bottom border, then adjust the appropriate boundary

                if(diffTop < diffBottom)
                {
                    // Drag the top of the item
                    m_drag_cursor_offset = diffTop;
                    m_drag_startDate = PointToDateTime(rect.topLeft());
                }
                else
                {
                    // Drag the bottom of the item
                    m_drag_cursor_offset = -diffBottom;
                    m_drag_endDate = PointToDateTime(rect.bottomRight());
                }
            }
            else
            {
                // Let them drag the index around
                m_drag_cursor_offset = diffTop;
                m_drag_startDate = PointToDateTime(rect.topLeft());
                m_drag_endDate = PointToDateTime(rect.bottomRight());

                setCursor(Qt::ClosedHandCursor);
            }
        }
        else
        {
            // If they didn't click on a valid index, then they can use a rubber
            //  band to select several indexes

            m_rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
            m_mouseFirstClick = event->pos();

            m_rubberBand->setGeometry(QRect(event->pos(), QSize()));
            m_rubberBand->show();
        }
    }
}

void TimelineView::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseReleaseEvent(event);

    if(!m_rubberBand.isNull())
        delete m_rubberBand.data();

    if(m_draggingIndex.isValid())
    {
        setCursor(Qt::OpenHandCursor);

        // Commit the data from the drag/drop session (Only if it changed)
        if(!m_drag_startDate.isNull() && m_drag_startDate != m_draggingIndex.data(StartDate))
            model()->setData(m_draggingIndex, m_drag_startDate, StartDate);
        if(!m_drag_endDate.isNull() && m_drag_endDate != m_draggingIndex.data(EndDate))
            model()->setData(m_draggingIndex, m_drag_endDate, EndDate);

        // Clear the drag/drop memory when we're done
        m_draggingIndex = QModelIndex();
        m_drag_startDate = QDateTime();
        m_drag_endDate = QDateTime();
        m_drag_cursor_offset = -1;
    }
}

void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseMoveEvent(event);

    QPoint pos(event->pos());
    if(m_rubberBand.isNull())
    {
        QRect rect;
        if(m_draggingIndex.isValid())
        {
            // In the middle of drag/drop
            _adjust_rect_for_dragDrop(rect = itemRect(m_draggingIndex));

            int dy( pos.y() + verticalScrollBar()->value() );

            if(m_drag_startDate.isNull())
                dy = dy - rect.bottom() - m_drag_cursor_offset;
            else
                dy = dy - rect.top() - m_drag_cursor_offset;

            if(dy != 0)
            {
                QDateTime effective_startDate( m_draggingIndex.data(StartDate).toDateTime() );
                QDateTime effective_endDate( m_draggingIndex.data(EndDate).toDateTime() );

                if(!m_drag_startDate.isNull())
                {
                    rect.setTop(rect.top() + dy);
                    effective_startDate = PointToDateTime(
                                QPoint( 0, rect.top() - verticalScrollBar()->value() ));
                }
                if(!m_drag_endDate.isNull())
                {
                    rect.setBottom(rect.bottom() + dy);
                    effective_endDate = PointToDateTime(
                                QPoint( 0, rect.bottom() - verticalScrollBar()->value() ));
                }

                // Only update the drag memory if it satisfies the validation constraints
                if(effective_endDate >= effective_startDate)
                {
                    if(!m_drag_startDate.isNull())
                        m_drag_startDate = effective_startDate;
                    if(!m_drag_endDate.isNull())
                        m_drag_endDate = effective_endDate;

                    viewport()->update();
                }
            }
        }
        else
        {
            rect = itemRect(indexAt(pos));
        }


        if(rect.isValid())
        {
            rect.translate(-horizontalScrollBar()->value(),
                           -verticalScrollBar()->value());

            if((gAbs(pos.y() - rect.top()) <= ADJUSTMENT_BUFFER ||
                gAbs(pos.y() - rect.bottom()) <= ADJUSTMENT_BUFFER))
            {
                setCursor(Qt::SizeVerCursor);
            }
            else if(pos.y() >= rect.top() && pos.y() <= rect.bottom())
            {
                setCursor(m_draggingIndex.isValid() ?
                              Qt::ClosedHandCursor : Qt::OpenHandCursor);
            }
        }
        else if(cursor().shape() != Qt::ArrowCursor)
        {
            setCursor(Qt::ArrowCursor);
        }
    }
    else
    {
        m_rubberBand->setGeometry(QRect(m_mouseFirstClick, pos).normalized());
    }
}

bool TimelineView::edit(const QModelIndex &, EditTrigger , QEvent *)
{
    return false;
}

void TimelineView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    viewport()->update();
}

QDateTime TimelineView::PointToDateTime(const QPoint &p) const
{
    return QDateTime(
                _start_time.addSecs(

                    // Determine the number of seconds to the point on the timeline
                    (long long)
                    (
                        // Find the percentage of distance the point lies
                        //  between the start/end points
                        //      (The '1' is a fudge factor, because there is an off-by-one
                        //      error between DateTimeToPoint and PointToDateTime)
                        ((double)(p.y() + 1 + verticalScrollBar()->value() - TIMELINE_TOPBOTTOM_MARGIN) /
                         (_finish_point.y() - _origin_point.y()))

                        // Multiply the percentage distance by the time duration on the
                        //  line.  We scale the time duration to account for
                        //  lower resolutions if the user wants it.
                        * (_range_in_seconds / _resolution_in_seconds)
                    )

                    // Scale the results back so it fits on the real timeline
                    * _resolution_in_seconds)
                );
}

QPoint TimelineView::DateTimeToPoint(const QDateTime &dt) const
{
    return QPoint(_origin_point.x(),
                  _origin_point.y() +
                  ((double)_start_time.secsTo(dt) / _range_in_seconds )
                  * (_finish_point.y() - _origin_point.y())
                  );
}

void TimelineView::_adjust_rect_for_dragDrop(QRect &r) const
{
    if(!m_drag_startDate.isNull())
        r.setTop(DateTimeToPoint(m_drag_startDate).y());

    if(!m_drag_endDate.isNull())
        r.setBottom(DateTimeToPoint(m_drag_endDate).y());
}

void TimelineView::SetTimeResolution(int t)
{
    _resolution_in_seconds = t;

    viewport()->update();
}
