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


protected slots:

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);


protected:

    virtual void paintEvent(QPaintEvent *);

    void wheelEvent(QWheelEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);


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
    QPoint _finish_point() const;

    QDateTime _start_time;
    QDateTime _end_time;
    int _range_in_seconds;

    // Memory for drawing the rubber band
    QPointer<QRubberBand> m_rubberBand;
    QPoint m_mouseFirstClick;

    QRubberBand m_currentHighlighter;

};


}}


#endif // TIMELINEVIEW_H
