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

#include "gutil_timelineview.h"
#include "gutil_symmetricmatrix.h"
#include "gutil_formattedtext.h"
#include "gutil_datetime.h"
#include <QPaintEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QApplication>
#include <QItemSelection>
#include <QRgb>
#include <QItemDelegate>

#include <QWheelEvent>
#include <QKeyEvent>
#include <QResizeEvent>

#include <QDateTimeEdit>

#define MIN_WIDTH 500
#define TIMELINE_TOPBOTTOM_MARGIN 11
#define TIMELINE_LEFT_MARGIN_12HOURS 85
#define TIMELINE_LEFT_MARGIN_24HOURS 70

USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL2(QT, DataObjects);

NAMESPACE_GUTIL2(QT, ModelViews);


TimelineView::TimelineView(QWidget *p)
    :QAbstractItemView(p),
      _p_DateTimeEditFormat("M/d/yyyy h:m:ss AP"),
      _p_AllowNewItems(true),
      _scale_factor(1),
      _origin_point(TIMELINE_LEFT_MARGIN_12HOURS, TIMELINE_TOPBOTTOM_MARGIN),
      _finish_point(_origin_point.x(), _origin_point.y()),
      _range_in_seconds(0),
      _resolution_in_seconds(1),
      m_dragRect(0),
      m_currentHighlighter(QRubberBand::Rectangle, this),
      m_drag_cursor_offset(-1),
      m_timeFormat(Format12Hours),
      m_dateTimeEdit(0)
{
    setMouseTracking(true);

    _update_finish_point();
}


void TimelineView::paintEvent(QPaintEvent *ev)
{
    QAbstractItemView::paintEvent(ev);

    QPainter p(viewport());
    p.setClipRegion(ev->region());
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

        fmt = QString("h:mm%1")
                .arg(GetTimeFormat() == Format12Hours ? " ap" : "");
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
        QModelIndexList allItems;
        for(int i = 0; i < model()->rowCount(); i++)
            allItems.append(model()->index(i, 0));

        int cnt(0);
        while(!allItems.isEmpty())
            _draw_items(allItems, p, cnt++);

        if(m_dragRect)
        {
            // Draw a white rectange for their drag rect
            const int middle_point( (m_dragRect->right() + m_dragRect->left()) / 2 );
            _draw_rect(m_dragRect->normalized(), Qt::white, QString("(New Item)"), p);

            const bool b_swap( m_dragRect->bottom() < m_dragRect->top() );
            _draw_datetime_rect(PointToDateTime(QPoint(0, m_dragRect->top() - verticalScrollBar()->value())),
                                QPoint(middle_point, m_dragRect->top()),
                                !b_swap, p);
            _draw_datetime_rect(PointToDateTime(QPoint(0, m_dragRect->bottom() - verticalScrollBar()->value())),
                                QPoint(middle_point, m_dragRect->bottom()),
                                b_swap, p);
        }

        if(!currentIndex().isValid())
            m_currentHighlighter.hide();
    }
}

void TimelineView::_draw_items(QModelIndexList &items,
                               QPainter &p,
                               int iteration)
{
    for(int i = items.count() - 1; i >= 0; i--)
    {
        QModelIndex ind( items[i] );
        if(ind.data(StartDate).toDateTime() >= _end_time ||
           ind.data(EndDate).toDateTime() <= _start_time)
            continue;

        if(m_draggingIndex == ind)
        {
            if(m_draggingIndex.isValid() &&
                    items.count() > 1)
                continue;
        }
        else if(_item_cache[ind].Position != iteration)
        {
            continue;
        }

        _draw_item(ind, p);
        items.removeAt(i);
    }
}


// The selected color
#define SELECTED_RGB 0xFF6699CC


void TimelineView::_draw_item(const QModelIndex &ind, QPainter &p)
{
    // The painter is being translated already to compensate for scrollbars, so
    //  we can use the real item rect without shifting it
    QRect item_rect( itemRect(ind) );

    // If they're in the middle of a drag/drop
    if(m_draggingIndex.isValid() &&
            m_draggingIndex == ind)
    {
        // First adjust the rect to the location to which they've dragged it
        _adjust_rect_for_dragDrop(item_rect);

        // Print the helpful datetime boxes
        if(!m_drag_startDate.isNull())
        {
            _draw_datetime_rect(m_drag_startDate,
                                QPoint(mapFromGlobal(QCursor::pos()).x(), item_rect.top()),
                                true,
                                p);
        }
        if(!m_drag_endDate.isNull())
        {
            _draw_datetime_rect(m_drag_endDate,
                                QPoint(mapFromGlobal(QCursor::pos()).x(), item_rect.bottom()),
                                false,
                                p);
        }
    }

    // Optimization: No need to draw an item that's outside the clipping region
    else if(!p.clipRegion().intersects(item_rect))
        return;

    QVariant color_variant = ind.data(Qt::BackgroundRole);
    QColor c;
    if(color_variant.isValid())
        c = color_variant.value<QColor>();
    else
        c = QColor(255, 255, 255);  // white

    if(selectionModel()->isSelected(ind))
        c = QColor(SELECTED_RGB);

    _draw_rect(item_rect, c,
               FormattedText(ind.data(Qt::DisplayRole).toString(),
                              ind.data(Qt::FontRole).value<QFont>()),
               p);

    if(ind == currentIndex())
    {
        m_currentHighlighter.setGeometry(item_rect.translated(viewport()->pos()).translated(
                                             -horizontalScrollBar()->value(), -verticalScrollBar()->value()));
        m_currentHighlighter.show();
    }
}

void TimelineView::_draw_rect(const QRect &r, const QColor &c, const FormattedText &txt, QPainter &p)
{
    p.fillRect(r, c);
    p.drawRect(r);

    p.save();
    {
        const int text_offset( 20 );
        p.setFont(txt.Font);
        p.drawText(r.left() + text_offset, r.top(), r.width() - text_offset, r.height(),
                   Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap,
                   txt.Text);
    }
    p.restore();
}

void TimelineView::_draw_datetime_rect(const QDateTime &dt, const QPoint &point, bool raise, QPainter &p)
{
    const int side_margin( 5 );
    QString fmt_string( dt.toString(QString("M/d/yyyy hh:mm:ss%1")
                                    .arg(GetTimeFormat() == Format12Hours ? " ap" : "")) );

    QRect bounding_rect( QFontMetrics(p.font()).boundingRect(fmt_string) );
    bounding_rect.adjust(-side_margin, 0, side_margin, 0);

    QRect textRect(QPoint(point.x() - bounding_rect.width() / 2,
                          point.y() + (raise ? -(bounding_rect.height() + 1) : 2 )),
                   bounding_rect.size());
    p.fillRect(textRect, Qt::white);
    p.drawRect(textRect);
    p.drawText(textRect, Qt::AlignHCenter, fmt_string);
}

QRect TimelineView::visualRect(const QModelIndex &index) const
{
    QRect rect( itemRect(index) );
    if(rect.isValid())
        rect.translate(-horizontalScrollBar()->value(),
                       -verticalScrollBar()->value());
    return rect;
}


#define TIMELINE_ITEM_WIDTH     400
#define TIMELINE_ITEM_OVERLAP   20

QRect TimelineView::itemRect(const QModelIndex &index) const
{
    if(!index.isValid())
        return QRect();

    QDateTime start( index.data(StartDate).toDateTime() );
    QDateTime end( index.data(EndDate).toDateTime() );

    if(start.isNull() || end.isNull())
        return QRect();

    QPoint item_start(DateTimeToPoint(start));
    QPoint item_end(item_start.x() + TIMELINE_ITEM_WIDTH,
                    DateTimeToPoint(end).y());

    // Adjust the points depending on if there are item conflicts
    ItemCache c( _item_cache[index] );

    if(c.TotalSections > 1)
    {
        // Adjust the left-hand X value
        item_start.setX(_origin_point.x() +
                        (((double)c.Position / c.TotalSections) * TIMELINE_ITEM_WIDTH) -
                        (c.Position == 0 ? 0 : TIMELINE_ITEM_OVERLAP));

        // Adjust the right-hand X value
        item_end.setX(item_start.x() +
                      (((double)c.Span / c.TotalSections) * TIMELINE_ITEM_WIDTH) +
                      (c.Position == 0 ? 0 : TIMELINE_ITEM_OVERLAP));
    }
    return QRect(item_start, item_end);
}

void TimelineView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    QRect area(viewport()->rect());
    QRect rect(visualRect(index));

    // Adjust the horizontal scrollbar
    {
        int hor(-1);
        if (rect.left() < area.left())
        {
            hor = horizontalScrollBar()->value() + rect.left() - area.left();


        }
        else if (rect.right() > area.right())
        {
            horizontalScrollBar()->setValue(
                        horizontalScrollBar()->value() + gMin(
                            rect.right() - area.right(), rect.left() - area.left()));
        }
        if(hor != -1)
            horizontalScrollBar()->setValue(hor);
    }


    // Adjust the vertical scrollbar
    if(rect.top() < area.top())
    {
        if(hint == EnsureVisible)
            hint = PositionAtTop;
    }
    else if(rect.bottom() > area.bottom())
    {
        if(hint == EnsureVisible)
            hint = PositionAtBottom;
    }
    else
        // The item is visible, so don't bother
        return;

    {
        int ver(-1);
        switch(hint)
        {
        case PositionAtTop:
            ver = verticalScrollBar()->value() + rect.top() - area.top();
            break;
        case PositionAtCenter:
            ver = verticalScrollBar()->value() +
                    (rect.bottom() + rect.top()) / 2 -
                    (area.bottom() + area.top()) / 2;
            break;
        case PositionAtBottom:
            ver = verticalScrollBar()->value() + gMin(rect.bottom() - area.bottom(), rect.top() - area.top());
            break;
        default:
            return;
        }
        if(ver != -1)
            verticalScrollBar()->setValue(ver);
    }
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

QModelIndex TimelineView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers)
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

bool TimelineView::isIndexHidden(const QModelIndex &) const
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
        double diff = SCROLL_INCREMENT * ev->delta();
        if(_scale_factor + diff > 0)
        {
            _scale_factor += diff;
            _update_finish_point();

            _update_scrollbars();
            viewport()->update();
            scrollTo(currentIndex(), EnsureVisible);
        }
    }
    else
    {
        verticalScrollBar()->setSliderPosition(
                verticalScrollBar()->sliderPosition() - (ev->delta() / 2));
    }
}

void TimelineView::keyPressEvent(QKeyEvent *event)
{
    event->ignore();

    if(event->modifiers().testFlag(Qt::ControlModifier))
    {
        if(event->key() == Qt::Key_0)
        {
            // Restore the default scale
            _scale_factor = 1;
            _update_finish_point();

            _update_scrollbars();
            viewport()->update();

            event->accept();
        }
    }

    if(!event->isAccepted())
        QAbstractItemView::keyPressEvent(event);
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


// Controls the distance from the top/bottom edges that you can click in to
//  start dragging an index
#define DRAG_ADJUSTMENT_MARGIN 10

void TimelineView::mousePressEvent(QMouseEvent *event)
{
    QAbstractItemView::mousePressEvent(event);

    if(event->button() == Qt::LeftButton)
    {
        QPoint pos( event->pos() );
        QModelIndex ind( indexAt(pos) );
        if(m_dateTimeEdit)
        {
            _commit_dateTime_editor();
        }
        else if(ind.isValid())
        {
            // If they clicked on a valid index, then we let them drag it around the timeline,
            //  or adjust the start/end date
            QRect rect( visualRect(ind) );
            int diffTop( gAbs(pos.y() - rect.top()) );
            int diffBottom( gAbs(pos.y() - rect.bottom()) );

            m_draggingIndex = ind;

            if(diffTop <= DRAG_ADJUSTMENT_MARGIN ||
                    diffBottom <= DRAG_ADJUSTMENT_MARGIN)
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

            viewport()->update();
        }
        else if(GetAllowNewItems())
        {
            // If they didn't click on a valid index, then they can drag to create
            //  a new item on the timeline (if this property is enabled)
            QPoint translated_pos(pos.x() + horizontalScrollBar()->value(),
                                  pos.y() + verticalScrollBar()->value());
            m_dragRect = new QRect(translated_pos, translated_pos);
        }
    }
}

void TimelineView::mouseReleaseEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseReleaseEvent(event);

    if(m_dragRect)
    {
        // For convenience, we'll check if the top and bottom are too close,
        //  'cause it's annoying if you accidentally clicked it and it creates an activity
        if(gAbs(m_dragRect->top() - m_dragRect->bottom()) > 2)
        {
            QDateTime
                    st(PointToDateTime(QPoint(0, m_dragRect->top() - verticalScrollBar()->value()))),
                    en(PointToDateTime(QPoint(0, m_dragRect->bottom() - verticalScrollBar()->value())));

            // Swap start/end time if the rect is backwards
            if(en < st)
            {
                QDateTime tt( st );
                st = en;
                en = tt;
            }

            // notify whoever's listening that the user wants to make a new item
            emit NewItemRequested(st, en);
        }

        delete m_dragRect;
        m_dragRect = 0;
    }
    else if(m_draggingIndex.isValid())
    {
        if(!m_drag_startDate.isNull() && !m_drag_endDate.isNull())
            setCursor(Qt::OpenHandCursor);

        // Commit the data from the drag/drop session (Only if it changed)
        QDateTime st, en;
        if(!m_drag_startDate.isNull() && m_drag_startDate != m_draggingIndex.data(StartDate))
            st = m_drag_startDate;
        if(!m_drag_endDate.isNull() && m_drag_endDate != m_draggingIndex.data(EndDate))
            en = m_drag_endDate;

        // For optimization we only want to call setdata once when setting the date,
        //  because redrawing is expensive
        if(!st.isNull() && !en.isNull())
        {
            QVariantList vl;
            vl.append(st);
            vl.append(en);
            model()->setData(m_draggingIndex, vl, StartAndEndDate);
        }
        else if(!st.isNull())
            model()->setData(m_draggingIndex, st, StartDate);
        else if(!en.isNull())
            model()->setData(m_draggingIndex, en, EndDate);

        // Clear the drag/drop memory when we're done
        m_draggingIndex = QModelIndex();
        m_drag_startDate = QDateTime();
        m_drag_endDate = QDateTime();
        m_drag_cursor_offset = -1;
    }

    viewport()->update();
}

void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseMoveEvent(event);

    QPoint pos(event->pos());
    if(m_dragRect)
    {
        // If they're dragging on the empty canvas to create a new item...
        m_dragRect->setBottomRight(QPoint(pos.x() + horizontalScrollBar()->value(),
                                          pos.y() + verticalScrollBar()->value()));
        viewport()->update();
    }
    else
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
                }
            }

            viewport()->update();
        }
        else
        {
            rect = itemRect(indexAt(pos));
        }


        if(rect.isValid())
        {
            rect.translate(-horizontalScrollBar()->value(),
                           -verticalScrollBar()->value());

            if((gAbs(pos.y() - rect.top()) <= DRAG_ADJUSTMENT_MARGIN ||
                gAbs(pos.y() - rect.bottom()) <= DRAG_ADJUSTMENT_MARGIN))
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
}

void TimelineView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint pos(event->pos());
    QModelIndex ind(indexAt(pos));
    if(ind.isValid())
    {
        if(m_editingIndex.isValid())
            event->ignore();
        else
        {
            QRect rect( visualRect(ind) );
            if(gAbs(pos.y() - rect.top()) < DRAG_ADJUSTMENT_MARGIN)
            {
                // Edit the start time
                m_editingData = StartDate;
                m_previousVerticalScrollbarPosition = verticalScrollBar()->value();
                m_previousHorizontalScrollbarPosition = horizontalScrollBar()->value();
                m_editingIndex = ind;

                m_dateTimeEdit = new QDateTimeEdit(ind.data(StartDate).toDateTime(), this);
                m_dateTimeEdit->setMouseTracking(true);
                m_dateTimeEdit->installEventFilter(this);
                m_dateTimeEdit->setDisplayFormat(GetDateTimeEditFormat());
                m_dateTimeEdit->move(QPoint(pos.x(), rect.top() - m_dateTimeEdit->height()));
                m_dateTimeEdit->show();

                event->ignore();
            }
            else if(gAbs(pos.y() - rect.bottom()) < DRAG_ADJUSTMENT_MARGIN)
            {
                // Edit the end time
                m_editingData = EndDate;
                m_previousVerticalScrollbarPosition = verticalScrollBar()->value();
                m_previousHorizontalScrollbarPosition = horizontalScrollBar()->value();
                m_editingIndex = ind;

                m_dateTimeEdit = new QDateTimeEdit(ind.data(EndDate).toDateTime(), this);
                m_dateTimeEdit->setMouseTracking(true);
                m_dateTimeEdit->installEventFilter(this);
                m_dateTimeEdit->setDisplayFormat(GetDateTimeEditFormat());
                m_dateTimeEdit->move(QPoint(pos.x(), rect.bottom()));
                m_dateTimeEdit->show();

                event->ignore();
            }
        }
    }

    if(event->isAccepted())
        QAbstractItemView::mouseDoubleClickEvent(event);
}

bool TimelineView::eventFilter(QObject *o, QEvent *ev)
{
    if(ev->type() == QEvent::KeyPress)
    {
        if(((QKeyEvent *)ev)->key() == Qt::Key_Return)
            _commit_dateTime_editor();
    }
    else if(ev->type() == QEvent::MouseMove)
    {
        // Make sure the cursor stays an arrow when hovering over a child widget
        if(cursor().shape() != Qt::IBeamCursor && cursor().shape() != Qt::ArrowCursor)
            setCursor(Qt::ArrowCursor);
    }

    return QAbstractItemView::eventFilter(o, ev);
}

void TimelineView::_commit_dateTime_editor()
{
    DataEnum de(m_editingData);
    QDateTime new_dateTime(m_dateTimeEdit->dateTime());
    QPersistentModelIndex ind(m_editingIndex);

    m_dateTimeEdit->hide();
    m_dateTimeEdit->deleteLater();
    m_dateTimeEdit = 0;
    m_editingIndex = QModelIndex();

    model()->setData(ind, new_dateTime, de);
}

bool TimelineView::edit(const QModelIndex &, EditTrigger , QEvent *)
{
    return false;
}

void TimelineView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight);

    if(m_dateTimeEdit)
    {
        for(int i = topLeft.row(); i <= bottomRight.row(); i++)
        {
            for(int j = topLeft.column(); j <= bottomRight.column(); j++)
            {
                // If the model changed the index we were editing, then throw away
                //  our editor
                if(model()->index(i, j) == m_editingIndex)
                {
                    m_dateTimeEdit->hide();
                    m_dateTimeEdit->deleteLater();
                    m_dateTimeEdit = 0;
                    m_editingIndex = QModelIndex();
                    break;
                }
            }
        }
    }

    // We reset, because any item might need a new layout
    reset();
}

void TimelineView::rowsInserted(const QModelIndex &par, int start, int end)
{
    QAbstractItemView::rowsInserted(par, start, end);

    // We reset, because any item might need a new layout
    reset();
}

void TimelineView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);

    if(m_dateTimeEdit)
    {
        for(int i = start; i <= end; i++)
        {
            if(model()->index(i, 0, parent) == m_editingIndex)
            {
                m_dateTimeEdit->hide();
                m_dateTimeEdit->deleteLater();
                m_dateTimeEdit = 0;
                m_editingIndex = QModelIndex();
                break;
            }
        }
    }
}

void TimelineView::reset()
{
    QAbstractItemView::reset();

    if(m_dateTimeEdit)
    {
        m_dateTimeEdit->hide();
        m_dateTimeEdit->deleteLater();
        m_dateTimeEdit = 0;
        m_editingIndex = QModelIndex();
    }

    // Refresh our cache of the timeline items
    _item_cache.clear();

    if(model())
    {
        const int rows(model()->rowCount());
        if(rows)
        {
            SymmetricMatrix<int> conflict_depth_matrix( rows, 0 );

            // Populate the first-order conflict depth matrix, and initialize the
            //  item cache while you're iterating through the rows anyways
            for(int i = 0; i < rows; i++)
            {
                QModelIndex ind(model()->index(i, 0));
                _item_cache.insert(ind, ItemCache());

                // Check all the rows after this one and remember which ones we conflict with
                for(int j = i - 1; j >= 0; j--)
                {
                    QModelIndex cmp(model()->index(j, 0));

                    // If the intersection of the time ranges is not null, then
                    //  the items conflict
                    if(Range<QDateTime>::Intersects(
                                Range<QDateTime>::CreateDoubleBound(ind.data(StartDate).toDateTime(),
                                                                    ind.data(EndDate).toDateTime()),
                                Range<QDateTime>::CreateDoubleBound(cmp.data(StartDate).toDateTime(),
                                                                    cmp.data(EndDate).toDateTime())))
                    {
                        ++ conflict_depth_matrix.Value(i, j);
                    }
                }
            }


            // Iteratively find the deeper conflicts
            SymmetricMatrix<int> m_tempmem(conflict_depth_matrix.RowCount(), 0);

            for(int col = 0; col < conflict_depth_matrix.ColumnCount(); col++)
            {
                for(int row = col + 1; row < conflict_depth_matrix.RowCount(); row++)
                {
                    int &val( conflict_depth_matrix.Value(row, col) );
                    if(val > 0)
                    {
                        m_tempmem.Value(row, col) = 1;

                        // Found a conflict, so search the conflictee and see if it conflicts
                        //  with another item that I also conflict with
                        for(int conflictRow = 0; conflictRow < conflict_depth_matrix.RowCount(); conflictRow++)
                        {
                            // Skip over indexes that would result in the 'Value()' calls
                            //  below to reference a cell on the diagonal
                            if(conflictRow == row ||
                                    conflictRow == col)
                                continue;

                            if(conflict_depth_matrix.Value(conflictRow, col) > 0 &&
                                    conflict_depth_matrix.Value(conflictRow, row) > 0)
                            {
                                // The conflict is higher order, so increment it
                                ++ val;
                            }
                        }
                    }
                }
            }

            forever
            {
                int r(-1), c(-1);

                // Find the most-conflicted
                const int max( conflict_depth_matrix.FindMaxValue(r, c) );
                int max_cnt( max );

                if(r == -1 || max == 0)
                    break;

                conflict_depth_matrix.Value(r, c) = 0;

                int &v( m_tempmem.Value(r, c) );
                if(v)
                {
                    _item_cache[model()->index(r, 0)].TotalSections = max + 1;

                    ItemCache &ic( _item_cache[model()->index(c, 0)] );
                    ic.TotalSections = max + 1;
                    ic.Position = max_cnt--;

                    // Flag that we've set the cache values
                    v = 0;
                }

                // Mark anyone who conflicts with me as the same total sections,
                //  and set their positions
                for(int i = 0; i < conflict_depth_matrix.ColumnCount(); i++)
                {
                    if(i != c && i != r &&
                            conflict_depth_matrix.Value(r, i) > 0)
                    {
                        int &tmpval( m_tempmem.Value(r, i) );
                        if(tmpval)
                        {
                            // And flag that we've set the TotalSections value here too
                            tmpval = 0;

                            ItemCache &c( _item_cache[model()->index(i, 0)] );
                            c.TotalSections = max + 1;
                            c.Position = max_cnt--;
                        }
                    }
                }
            }
        }
    }
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

void TimelineView::verticalScrollbarValueChanged(int value)
{
    QAbstractItemView::verticalScrollbarValueChanged(value);

    if(m_dateTimeEdit)
        m_dateTimeEdit->move(QPoint(m_dateTimeEdit->pos().x(),
                                    m_dateTimeEdit->pos().y() + m_previousVerticalScrollbarPosition - value));

    m_previousVerticalScrollbarPosition = value;
}

void TimelineView::horizontalScrollbarValueChanged(int value)
{
    QAbstractItemView::horizontalScrollbarValueChanged(value);

    if(m_dateTimeEdit)
        m_dateTimeEdit->move(QPoint(m_dateTimeEdit->pos().x() +  m_previousHorizontalScrollbarPosition - value,
                                    m_dateTimeEdit->pos().y()));

    m_previousHorizontalScrollbarPosition = value;
}

void TimelineView::setModel(QAbstractItemModel *m)
{
    if(model())
        disconnect(model(), SIGNAL(rowsRemoved(QModelIndex, int, int)),
                   this, SLOT(_model_rows_removed()));

    if(m)
        connect(m, SIGNAL(rowsRemoved(QModelIndex, int, int)),
                this, SLOT(_model_rows_removed()));

    QAbstractItemView::setModel(m);
}

void TimelineView::_model_rows_removed()
{
    // We have to reset our view, so we can redraw the items
    reset();
}

void TimelineView::SetTimeFormat(TimeFormatEnum t)
{
    m_timeFormat = t;

    const int newx(t == Format12Hours ?
                       TIMELINE_LEFT_MARGIN_12HOURS :
                       TIMELINE_LEFT_MARGIN_24HOURS);
    _origin_point.setX(newx);
    _finish_point.setX(newx);

    repaint();
}


END_NAMESPACE_GUTIL2;
