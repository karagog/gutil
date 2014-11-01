/*Copyright 2014-2015 George Karagoulis

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

#include <gutil/macros.h>
#include <math.h>
NAMESPACE_GUTIL;


/** Implements the 3-dimensional free EuclideanVector.
 *
 *  This vector has its base at the origin.
*/
template<class T> class EuclideanVector3
{
public:
    /** Constructs a null vector. */
    EuclideanVector3() :_p_X(0), _p_Y(0), _p_Z(0) {}

    /** Constructs a vector with the given x, y and z components. */
    EuclideanVector3(const T &x, const T &y, const T &z) :_p_X(x), _p_Y(y), _p_Z(z) {}

    /** Constructs a vector from the given two points, based at the origin. */
    EuclideanVector3(const T &x1, const T &y1, const T &z1,
                     const T &x2, const T &y2, const T &z2)
        :_p_X(x2-x1), _p_Y(y2-y1), _p_Z(z2-z1) {}

    PROPERTY(X, T);
    PROPERTY(Y, T);
    PROPERTY(Z, T);

    /** Returns true if this is a null vector (0 magnitude). */
    bool IsNull() const{ return GetX() == 0 && GetY() == 0 && GetZ() == 0; }

    /** Returns the scalar magnitude of the vector. */
    T Magnitude() const{ return sqrt(GetX()*GetX() + GetY()*GetY() + GetZ()*GetZ()); }

    /** Returns the unit vector of the vector. */
    EuclideanVector3<T> UnitVector () const{ return *this/Magnitude(); }

    /** Multiplies the vector by the scalar. */
    EuclideanVector3<T> operator * (const T &val) const{ return EuclideanVector3<T>(*this) *= val; }
    EuclideanVector3<T> &operator *= (const T &val){
        SetX(GetX()*val);
        SetY(GetY()*val);
        SetZ(GetZ()*val);
        return *this;
    }

    /** Gives you the dot product of the vectors. */
    T operator * (const EuclideanVector3 &o) const{ return GetX()*o.GetX() + GetY()*o.GetY() + GetZ()*o.GetZ(); }

    /** Gives you the cross-product of the two vectors. */
    EuclideanVector3<T> operator ^ (const EuclideanVector3<T> &o) const{
        return EuclideanVector3<T>(GetY()*o.GetZ()-GetZ()*o.GetY(),
                                GetZ()*o.GetX()-GetX()*o.GetZ(),
                                GetX()*o.GetY()-GetY()*o.GetX());
    }

    /** Divides the vector by the scalar. */
    EuclideanVector3<T> operator / (const T &val) const{ return EuclideanVector3<T>(*this) /= val; }
    EuclideanVector3<T> &operator /= (const T &val){
        SetX(GetX()/val);
        SetY(GetY()/val);
        SetZ(GetZ()/val);
        return *this;
    }

    /** Adds the other vector to this one. */
    EuclideanVector3<T> operator + (const EuclideanVector3<T> &o) const{ return EuclideanVector3<T>(*this) += o; }
    EuclideanVector3<T> &operator += (const EuclideanVector3<T> &o){
        SetX(GetX()+o.GetX());
        SetY(GetY()+o.GetY());
        SetZ(GetZ()+o.GetZ());
        return *this;
    }

    /** Subtracts the other vector from this one. */
    EuclideanVector3<T> operator - (const EuclideanVector3<T> &o) const{ return EuclideanVector3<T>(*this) -= o; }
    EuclideanVector3<T> &operator -= (const EuclideanVector3<T> &o){
        SetX(GetX()-o.GetX());
        SetY(GetY()-o.GetY());
        SetZ(GetZ()-o.GetZ());
        return *this;
    }

    /** Returns the negative of this vector. */
    EuclideanVector3<T> operator - () const{
        return EuclideanVector3<T>(-GetX(), -GetY(), -GetZ());
    }

};


/** Implements the 2-dimensional free EuclideanVector.
 *
 *  This vector has its base at the origin.
*/
template<class T> class EuclideanVector2
{
public:
    /** Constructs a null vector. */
    EuclideanVector2() :_p_X(0), _p_Y(0) {}

    /** Constructs a vector with the given x and y components. */
    EuclideanVector2(const T &x, const T &y) :_p_X(x), _p_Y(y) {}

    /** Constructs a vector from the given two points, based at the origin. */
    EuclideanVector2(const T &x1, const T &y1,
                     const T &x2, const T &y2)
        :_p_X(x2-x1), _p_Y(y2-y1) {}

    PROPERTY(X, T);
    PROPERTY(Y, T);

    /** Returns true if this is a null vector (0 magnitude). */
    bool IsNull() const{ return GetX() == 0 && GetY() == 0; }

    /** Returns the scalar magnitude of the vector. */
    T Magnitude() const{ return sqrt(GetX()*GetX() + GetY()*GetY()); }

    /** Returns a perpendicular vector to this one. You can negate this vector
        to get the other perpendicular one.
    */
    EuclideanVector2<T> Orthogonal() const{ return EuclideanVector2<T>(-GetY(), GetX()); }

    /** Returns the unit vector of the vector. */
    EuclideanVector2<T> UnitVector () const{ return *this/Magnitude(); }

    /** Multiplies the vector by the scalar. */
    EuclideanVector2<T> operator * (const T &val) const{ return EuclideanVector2<T>(*this) *= val; }
    EuclideanVector2<T> &operator *= (const T &val){
        SetX(GetX()*val);
        SetY(GetY()*val);
        return *this;
    }

    /** Gives you the dot product of the vectors. */
    T operator * (const EuclideanVector2<T> &o) const{ return GetX()*o.GetX() + GetY()*o.GetY(); }

    /** Gives you the cross-product of the two vectors, which is a 3-d vector. */
    EuclideanVector3<T> operator ^ (const EuclideanVector2<T> &o) const{ return EuclideanVector3<T>(0, 0, GetX()*o.GetY() - GetY()*o.GetX()); }

    /** Divides the vector by the scalar. */
    EuclideanVector2<T> operator / (const T &val) const{ return EuclideanVector2<T>(*this) /= val; }
    EuclideanVector2<T> &operator /= (const T &val){
        SetX(GetX()/val);
        SetY(GetY()/val);
        return *this;
    }

    /** Adds the other vector to this one. */
    EuclideanVector2<T> operator + (const EuclideanVector2<T> &o) const{ return EuclideanVector2<T>(*this) += o; }
    EuclideanVector2<T> &operator += (const EuclideanVector2<T> &o){
        SetX(GetX()+o.GetX());
        SetY(GetY()+o.GetY());
        return *this;
    }

    /** Subtracts the other vector from this one. */
    EuclideanVector2<T> operator - (const EuclideanVector2<T> &o) const{ return EuclideanVector2<T>(*this) -= o; }
    EuclideanVector2<T> &operator -= (const EuclideanVector2<T> &o){
        SetX(GetX()-o.GetX());
        SetY(GetY()-o.GetY());
        return *this;
    }

    /** Returns the negative of this vector. */
    EuclideanVector2<T> operator - () const{ return EuclideanVector2<T>(-GetX(), -GetY()); }

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_EUCLIDEANVECTOR_H
