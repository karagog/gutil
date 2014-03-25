/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "paintutils.h"
#include "gutil_euclideanvector.h"
#include "gutil_exception.h"
#include <math.h>
#include <QPainter>

NAMESPACE_GUTIL1(QT);


void PaintUtils::DrawArrow(QPainter &painter,
                           const QPointF &start,
                           const QPointF &end,
                           int bevel_length,
                           GFLOAT64 bevel_angle,
                           const QColor &color,
                           int thickness)
{
    if(bevel_angle >= 90)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "No angles over 90 degrees");

    painter.save();
    QPen pen(color);
    pen.setWidth(thickness);
    painter.setPen(pen);
    
    // Draw the arrow main line
    painter.drawLine(start, end);
    
    // Draw the pointy part of the arrow

    // First create a vector from the two points and find its unit vector
    EuclideanVector2 v(start.x(), start.y(), end.x(), end.y());
    EuclideanVector2 unit = v.UnitVector();

    // Find the base of the point triangle
    EuclideanVector2 bevel_base(v - (unit * (cos(bevel_angle*M_PI/180)*bevel_length)));

    // Find the orthogonal vector which takes us to the two points of the triangle
    EuclideanVector2 d = unit.Orthogonal()*(sin(bevel_angle*M_PI/180)*bevel_length);

    // The two points we need are the bevel_base +- the orthogonal vector
    EuclideanVector2 pt = bevel_base + d;
    
    // Don't forget to offset by the start point, since the euclidean vector is based at the origin
    painter.drawLine(end, QPointF(start.x() + pt.GetX(), start.y() + pt.GetY()));
    
    // Draw the line on the opposite side
    pt = bevel_base - d;
    painter.drawLine(end, QPointF(start.x() + pt.GetX(), start.y() + pt.GetY()));

    painter.restore();
}


END_NAMESPACE_GUTIL1;
