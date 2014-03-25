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

#ifndef GUTIL_PAINTUTILS_H
#define GUTIL_PAINTUTILS_H

#include "gutil_macros.h"
#include <QColor>

class QPainter;
class QPointF;

NAMESPACE_GUTIL1(QT);


/** A class that defines functions which are useful for
    painting within Qt's Widget system.
    
    \note All painting functions must be called with a QPainter that was
    initialized within the QWidget's paintEvent().
*/
class PaintUtils
{
public:

    /** Draws an arrow from the start point to the end point, with the given
        parameters describing the color and geometry.
        
        \param start The start point relative to the viewport of the paint device.
        \param end The end point relative to the viewport of the paint device.
        \param bevel_length The length of the "pointy" part of the arrow, in pixels
        \param bevel_angle The angle the "pointy" part of the arrow makes, in degrees. This must be less than 90.
        \param rgba The rgba value for the color you want to paint. The default is black.
        \param thickness The thickness of the arrow
    */
    static void DrawArrow(QPainter &,
                          const QPointF &start,
                          const QPointF &end,
                          int bevel_length,
                          GFLOAT64 bevel_angle = 30,
                          const QColor &color = Qt::black,
                          int thickness = 5);

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_PAINTUTILS_H
