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

#include <math.h>
#include "gutil_macros.h"
NAMESPACE_GUTIL;


/** Declares a euclidean vector with the given dimensions.
 *
 *  The default is a 2-d vector
*/
template<int dimensions = 2>class EuclideanVector;


/** Implements the 3-dimensional free EuclideanVector.
 *
 *  This vector has its base at the origin.
*/
template<>
class EuclideanVector<3>
{
public:
    /** Constructs a null vector. */
    EuclideanVector():_p_X(0), _p_Y(0), _p_Z(0){}

    /** Constructs a vector with the given x, y and z components. */
    EuclideanVector(GFLOAT64 x, GFLOAT64 y, GFLOAT64 z):_p_X(x), _p_Y(y), _p_Z(z){}

    /** Constructs a vector from the given two points, based at the origin. */
    EuclideanVector(GFLOAT64 x1, GFLOAT64 y1, GFLOAT64 z1,
                    GFLOAT64 x2, GFLOAT64 y2, GFLOAT64 z2)
        :_p_X(x2-x1), _p_Y(y2-y1), _p_Z(z2-z1){}

    PROPERTY(X, GFLOAT64);
    PROPERTY(Y, GFLOAT64);
    PROPERTY(Z, GFLOAT64);

    /** Returns true if this is a null vector (0 magnitude). */
    bool IsNull() const{ return 0 == GetX() && 0 == GetY() && 0 == GetZ(); }

    /** Returns the scalar magnitude of the vector. */
    GFLOAT64 Magnitude() const{ return sqrt(GetX()*GetX() + GetY()*GetY() + GetZ()*GetZ()); }

    /** Returns the unit vector of the vector. */
    EuclideanVector<3> UnitVector () const{ return *this/Magnitude(); }

    /** Multiplies the vector by the scalar. */
    EuclideanVector<3> operator * (GFLOAT64 val) const{
        return EuclideanVector<3>(*this) *= val;
    }
    EuclideanVector<3> &operator *= (GFLOAT64 val){
        SetX(GetX()*val);
        SetY(GetY()*val);
        SetZ(GetZ()*val);
        return *this;
    }

    /** Gives you the dot product of the vectors. */
    GFLOAT64 operator * (const EuclideanVector<3> &o) const{
        return GetX()*o.GetX() + GetY()*o.GetY() + GetZ()*o.GetZ();
    }

    /** Gives you the cross-product of the two vectors. */
    EuclideanVector<3> operator ^ (const EuclideanVector<3> &o) const{
        return EuclideanVector<3>(GetY()*o.GetZ()-GetZ()*o.GetY(),
                                         GetZ()*o.GetX()-GetX()*o.GetZ(),
                                         GetX()*o.GetY()-GetY()*o.GetX());
    }

    /** Divides the vector by the scalar. */
    EuclideanVector<3> operator / (GFLOAT64 val) const{
        return EuclideanVector<3>(*this) /= val;
    }
    EuclideanVector<3> &operator /= (GFLOAT64 val){
        SetX(GetX()/val);
        SetY(GetY()/val);
        SetZ(GetZ()/val);
        return *this;
    }

    /** Adds the other vector to this one. */
    EuclideanVector<3> operator + (const EuclideanVector<3> &o) const{
        return EuclideanVector<3>(*this) += o;
    }
    EuclideanVector<3> &operator += (const EuclideanVector<3> &o){
        SetX(GetX()+o.GetX());
        SetY(GetY()+o.GetY());
        SetZ(GetZ()+o.GetZ());
        return *this;
    }

    /** Subtracts the other vector from this one. */
    EuclideanVector<3> operator - (const EuclideanVector<3> &o) const{
        return EuclideanVector<3>(*this) -= o;
    }
    EuclideanVector<3> &operator -= (const EuclideanVector<3> &o){
        SetX(GetX()-o.GetX());
        SetY(GetY()-o.GetY());
        SetZ(GetZ()-o.GetZ());
        return *this;
    }

    /** Returns the negative of this vector. */
    EuclideanVector<3> operator - (){ return EuclideanVector<3>(-GetX(), -GetY(), -GetZ()); }

};


/** Implements the 2-dimensional free EuclideanVector.
 *
 *  This vector has its base at the origin.
*/
template<>
class EuclideanVector<2>
{
public:
    /** Constructs a null vector. */
    EuclideanVector():_p_X(0), _p_Y(0){}

    /** Constructs a vector with the given x and y components. */
    EuclideanVector(GFLOAT64 x, GFLOAT64 y):_p_X(x), _p_Y(y){}

    /** Constructs a vector from the given two points, based at the origin. */
    EuclideanVector(GFLOAT64 x1, GFLOAT64 y1,
                    GFLOAT64 x2, GFLOAT64 y2)
        :_p_X(x2-x1), _p_Y(y2-y1){}

    PROPERTY(X, GFLOAT64);
    PROPERTY(Y, GFLOAT64);

    /** Returns true if this is a null vector (0 magnitude). */
    bool IsNull() const{ return 0 == GetX() && 0 == GetY(); }

    /** Returns the scalar magnitude of the vector. */
    GFLOAT64 Magnitude() const{ return sqrt(GetX()*GetX() + GetY()*GetY()); }

    /** Returns a perpendicular vector to this one. */
    EuclideanVector<2> Orthogonal() const{ return EuclideanVector<2>(-GetY(), GetX()); }

    /** Returns the unit vector of the vector. */
    EuclideanVector<2> UnitVector () const{ return *this/Magnitude(); }

    /** Multiplies the vector by the scalar. */
    EuclideanVector<2> operator * (GFLOAT64 val) const{
        return EuclideanVector<2>(*this) *= val;
    }
    EuclideanVector<2> &operator *= (GFLOAT64 val){
        SetX(GetX()*val);
        SetY(GetY()*val);
        return *this;
    }

    /** Gives you the dot product of the vectors. */
    GFLOAT64 operator * (const EuclideanVector<2> &o) const{
        return GetX()*o.GetX() + GetY()*o.GetY();
    }

    /** Gives you the cross-product of the two vectors. */
    EuclideanVector<3> operator ^ (const EuclideanVector<2> &o) const{
        return EuclideanVector<3>(0, 0, GetX()*o.GetY() - GetY()*o.GetX());
    }

    /** Divides the vector by the scalar. */
    EuclideanVector<2> operator / (GFLOAT64 val) const{
        return EuclideanVector<2>(*this) /= val;
    }
    EuclideanVector<2> &operator /= (GFLOAT64 val){
        SetX(GetX()/val);
        SetY(GetY()/val);
        return *this;
    }

    /** Adds the other vector to this one. */
    EuclideanVector<2> operator + (const EuclideanVector<2> &o) const{
        return EuclideanVector<2>(*this) += o;
    }
    EuclideanVector<2> &operator += (const EuclideanVector<2> &o){
        SetX(GetX()+o.GetX());
        SetY(GetY()+o.GetY());
        return *this;
    }

    /** Subtracts the other vector from this one. */
    EuclideanVector<2> operator - (const EuclideanVector<2> &o) const{
        return EuclideanVector<2>(*this) -= o;
    }
    EuclideanVector<2> &operator -= (const EuclideanVector<2> &o){
        SetX(GetX()-o.GetX());
        SetY(GetY()-o.GetY());
        return *this;
    }

    /** Returns the negative of this vector. */
    EuclideanVector<2> operator - (){ return EuclideanVector<2>(-GetX(), -GetY()); }

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_EUCLIDEANVECTOR_H
