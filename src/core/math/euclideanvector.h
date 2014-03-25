/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_EUCLIDEANVECTOR_H
#define GUTIL_EUCLIDEANVECTOR_H

#include "gutil_macros.h"
NAMESPACE_GUTIL;


/** Implements the 3-dimensional free EuclideanVector.
 *
 *  This vector has its base at the origin.
*/
class EuclideanVector3
{
public:
    /** Constructs a null vector. */
    EuclideanVector3();

    /** Constructs a vector with the given x, y and z components. */
    EuclideanVector3(GFLOAT64 x, GFLOAT64 y, GFLOAT64 z);

    /** Constructs a vector from the given two points, based at the origin. */
    EuclideanVector3(GFLOAT64 x1, GFLOAT64 y1, GFLOAT64 z1,
                     GFLOAT64 x2, GFLOAT64 y2, GFLOAT64 z2);

    PROPERTY(X, GFLOAT64);
    PROPERTY(Y, GFLOAT64);
    PROPERTY(Z, GFLOAT64);

    /** Returns true if this is a null vector (0 magnitude). */
    bool IsNull() const;

    /** Returns the scalar magnitude of the vector. */
    GFLOAT64 Magnitude() const;

    /** Returns the unit vector of the vector. */
    EuclideanVector3 UnitVector () const;

    /** Multiplies the vector by the scalar. */
    EuclideanVector3 operator * (GFLOAT64 val) const{ return EuclideanVector3(*this) *= val; }
    EuclideanVector3 &operator *= (GFLOAT64 val);

    /** Gives you the dot product of the vectors. */
    GFLOAT64 operator * (const EuclideanVector3 &o) const;

    /** Gives you the cross-product of the two vectors. */
    EuclideanVector3 operator ^ (const EuclideanVector3 &o) const;

    /** Divides the vector by the scalar. */
    EuclideanVector3 operator / (GFLOAT64 val) const{ return EuclideanVector3(*this) /= val; }
    EuclideanVector3 &operator /= (GFLOAT64 val);

    /** Adds the other vector to this one. */
    EuclideanVector3 operator + (const EuclideanVector3 &o) const{ return EuclideanVector3(*this) += o; }
    EuclideanVector3 &operator += (const EuclideanVector3 &o);

    /** Subtracts the other vector from this one. */
    EuclideanVector3 operator - (const EuclideanVector3 &o) const{ return EuclideanVector3(*this) -= o; }
    EuclideanVector3 &operator -= (const EuclideanVector3 &o);

    /** Returns the negative of this vector. */
    EuclideanVector3 operator - ();

};


/** Implements the 2-dimensional free EuclideanVector.
 *
 *  This vector has its base at the origin.
*/
class EuclideanVector2
{
public:
    /** Constructs a null vector. */
    EuclideanVector2();

    /** Constructs a vector with the given x and y components. */
    EuclideanVector2(GFLOAT64 x, GFLOAT64 y);

    /** Constructs a vector from the given two points, based at the origin. */
    EuclideanVector2(GFLOAT64 x1, GFLOAT64 y1,
                     GFLOAT64 x2, GFLOAT64 y2);

    PROPERTY(X, GFLOAT64);
    PROPERTY(Y, GFLOAT64);

    /** Returns true if this is a null vector (0 magnitude). */
    bool IsNull() const;

    /** Returns the scalar magnitude of the vector. */
    GFLOAT64 Magnitude() const;

    /** Returns a perpendicular vector to this one. You can negate this vector
        to get the other perpendicular one.
    */
    EuclideanVector2 Orthogonal() const;

    /** Returns the unit vector of the vector. */
    EuclideanVector2 UnitVector () const;

    /** Multiplies the vector by the scalar. */
    EuclideanVector2 operator * (GFLOAT64 val) const{ return EuclideanVector2(*this) *= val; }
    EuclideanVector2 &operator *= (GFLOAT64 val);

    /** Gives you the dot product of the vectors. */
    GFLOAT64 operator * (const EuclideanVector2 &o) const;

    /** Gives you the cross-product of the two vectors, which is a 3-d vector. */
    EuclideanVector3 operator ^ (const EuclideanVector2 &o) const;

    /** Divides the vector by the scalar. */
    EuclideanVector2 operator / (GFLOAT64 val) const{ return EuclideanVector2(*this) /= val; }
    EuclideanVector2 &operator /= (GFLOAT64 val);

    /** Adds the other vector to this one. */
    EuclideanVector2 operator + (const EuclideanVector2 &o) const{ return EuclideanVector2(*this) += o; }
    EuclideanVector2 &operator += (const EuclideanVector2 &o);

    /** Subtracts the other vector from this one. */
    EuclideanVector2 operator - (const EuclideanVector2 &o) const{ return EuclideanVector2(*this) -= o; }
    EuclideanVector2 &operator -= (const EuclideanVector2 &o);

    /** Returns the negative of this vector. */
    EuclideanVector2 operator - ();

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_EUCLIDEANVECTOR_H
