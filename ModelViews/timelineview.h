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

#ifndef TIMELINEVIEW_H
#define TIMELINEVIEW_H

#include <QAbstractItemView>
#include <QPointer>
#include <QRubberBand>
#include <QDateTime>
#include "gutil_macros.h"

class QDateTimeEdit;


namespace GUtil{ namespace ModelViews{


class TimelineView :
        public QAbstractItemView
{
    Q_OBJECT
public:

    explicit TimelineView(QWidget *parent = 0);

    void SetStartTime(const QDateTime &);
    void SetEndTime(const QDateTime &);
    void SetStartAndEndTime(const QDateTime &start, const QDateTime &end);

    // Expects a point that has been shifted due to the vertical scrollbar
    QDateTime PointToDateTime(const QPoint &) const;

    // Returns a point that has NOT been shifted due to the vertical scrollbar
    QPoint DateTimeToPoint(const QDateTime &) const;

    // The amount of time (in seconds) that we'll allow the user to edit
    inline int GetTimeResolution() const{
        return _resolution_in_seconds;
    }
    void SetTimeResolution(int);

    // Controls the format they use to edit the datetimes
    PROPERTY( DateTimeEditFormat, QString );

    // The view will use these role values to reference data in the model,
    //   using the 'data()' function
    enum DataEnum
    {
        StartDate = Qt::UserRole + 25,
        EndDate = Qt::UserRole + 26
    };

    // QAbstractItemView interface:
    virtual QModelIndex indexAt(const QPoint &point) const;
    virtual QRect visualRect(const QModelIndex &index) const;
    virtual void scrollTo(const QModelIndex &index, ScrollHint hint);


public slots:

    virtual void reset();


protected slots:

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void rowsInserted(const QModelIndex &parent, int start, int end);
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

    void verticalScrollbarValueChanged(int value);
    void horizontalScrollbarValueChanged(int value);


protected:

    virtual void paintEvent(QPaintEvent *);

    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

    bool eventFilter(QObject *, QEvent *);

    bool edit(const QModelIndex &index, EditTrigger trigger, QEvent *event);


    // QAbstractItemView interface:
    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    virtual int horizontalOffset() const;
    virtual int verticalOffset() const;
    virtual bool isIndexHidden(const QModelIndex &index) const;
    virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    virtual QRegion visualRegionForSelection(const QItemSelection &selection) const;

    virtual void scrollContentsBy(int dx, int dy);


private:

    double _scale_factor;

    void _draw_item(const QModelIndex &, QPainter &);

    QRegion itemRegion(const QModelIndex &) const;

    // Returns the un-translated item rect (without taking scrollbars into account
    QRect itemRect(const QModelIndex &) const;

    void _update_scrollbars();

    const QPoint _origin_point;
    QPoint _finish_point;

    inline void _update_finish_point(){
        _finish_point.setY(_scale_factor * 500);
    }

    QDateTime _start_time;
    QDateTime _end_time;
    long long _range_in_seconds;

    // Defines the minimum time resolution we'll allow
    int _resolution_in_seconds;

    // Memory for drawing the rubber band
    QPointer<QRubberBand> m_rubberBand;
    QPoint m_mouseFirstClick;

    QRubberBand m_currentHighlighter;

    // For remembering while they drag an activity around
    QDateTime m_drag_startDate;
    QDateTime m_drag_endDate;
    QModelIndex m_draggingIndex;
    // While dragging, this is the cursor's offset from the top of the item's rect
    int m_drag_cursor_offset;

    void _adjust_rect_for_dragDrop(QRect &) const;

    QPersistentModelIndex m_editingIndex;
    DataEnum m_editingData;
    int m_previousVerticalScrollbarPosition;
    int m_previousHorizontalScrollbarPosition;
    QDateTimeEdit *m_dateTimeEdit;
    void _commit_dateTime_editor();

};


}}


#endif // TIMELINEVIEW_H
