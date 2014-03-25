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

#include "euclideanvector.h"
#include <math.h>
NAMESPACE_GUTIL;


EuclideanVector3::EuclideanVector3()
    :_p_X(0), _p_Y(0), _p_Z(0){}

EuclideanVector3::EuclideanVector3(GFLOAT64 x, GFLOAT64 y, GFLOAT64 z)
    :_p_X(x), _p_Y(y), _p_Z(z){}
    
EuclideanVector3::EuclideanVector3(GFLOAT64 x1, GFLOAT64 y1, GFLOAT64 z1,
                                GFLOAT64 x2, GFLOAT64 y2, GFLOAT64 z2)
    :_p_X(x2-x1), _p_Y(y2-y1), _p_Z(z2-z1){}
    
bool EuclideanVector3::IsNull() const
{ 
    return 0 == GetX() && 0 == GetY() && 0 == GetZ(); 
}

GFLOAT64 EuclideanVector3::Magnitude() const
{
    return sqrt(GetX()*GetX() + GetY()*GetY() + GetZ()*GetZ()); 
}

EuclideanVector3 EuclideanVector3::UnitVector() const
{
    return *this/Magnitude();
}

EuclideanVector3 &EuclideanVector3::operator *= (GFLOAT64 val)
{
    SetX(GetX()*val);
    SetY(GetY()*val);
    SetZ(GetZ()*val);
    return *this;
}

GFLOAT64 EuclideanVector3::operator * (const EuclideanVector3 &o) const
{
    return GetX()*o.GetX() + GetY()*o.GetY() + GetZ()*o.GetZ();
}

EuclideanVector3 EuclideanVector3::operator ^ (const EuclideanVector3 &o) const
{
    return EuclideanVector3(GetY()*o.GetZ()-GetZ()*o.GetY(),
                            GetZ()*o.GetX()-GetX()*o.GetZ(),
                            GetX()*o.GetY()-GetY()*o.GetX());
}

EuclideanVector3 &EuclideanVector3::operator /= (GFLOAT64 val)
{
    SetX(GetX()/val);
    SetY(GetY()/val);
    SetZ(GetZ()/val);
    return *this;
}

EuclideanVector3 &EuclideanVector3::operator += (const EuclideanVector3 &o)
{
    SetX(GetX()+o.GetX());
    SetY(GetY()+o.GetY());
    SetZ(GetZ()+o.GetZ());
    return *this;
}

EuclideanVector3 &EuclideanVector3::operator -= (const EuclideanVector3 &o)
{
    SetX(GetX()-o.GetX());
    SetY(GetY()-o.GetY());
    SetZ(GetZ()-o.GetZ());
    return *this;
}

EuclideanVector3 EuclideanVector3::operator - ()
{
    return EuclideanVector3(-GetX(), -GetY(), -GetZ());
}




EuclideanVector2::EuclideanVector2()
    :_p_X(0), _p_Y(0){}
    
EuclideanVector2::EuclideanVector2(GFLOAT64 x, GFLOAT64 y)
    :_p_X(x), _p_Y(y){}
    
EuclideanVector2::EuclideanVector2(GFLOAT64 x1, GFLOAT64 y1,
                                    GFLOAT64 x2, GFLOAT64 y2)
    :_p_X(x2-x1), _p_Y(y2-y1){}

bool EuclideanVector2::IsNull() const
{
    return 0 == GetX() && 0 == GetY();
}

GFLOAT64 EuclideanVector2::Magnitude() const
{
    return sqrt(GetX()*GetX() + GetY()*GetY());
}

EuclideanVector2 EuclideanVector2::Orthogonal() const
{
    return EuclideanVector2(-GetY(), GetX());
}

EuclideanVector2 EuclideanVector2::UnitVector () const
{
    return *this/Magnitude();
}

EuclideanVector2 &EuclideanVector2::operator *= (GFLOAT64 val)
{
    SetX(GetX()*val);
    SetY(GetY()*val);
    return *this;
}

GFLOAT64 EuclideanVector2::operator * (const EuclideanVector2 &o) const
{
    return GetX()*o.GetX() + GetY()*o.GetY();
}

EuclideanVector3 EuclideanVector2::operator ^ (const EuclideanVector2 &o) const
{
    return EuclideanVector3(0, 0, GetX()*o.GetY() - GetY()*o.GetX());
}

EuclideanVector2 &EuclideanVector2::operator /= (GFLOAT64 val)
{
    SetX(GetX()/val);
    SetY(GetY()/val);
    return *this;
}

EuclideanVector2 &EuclideanVector2::operator += (const EuclideanVector2 &o)
{
    SetX(GetX()+o.GetX());
    SetY(GetY()+o.GetY());
    return *this;
}

EuclideanVector2 &EuclideanVector2::operator -= (const EuclideanVector2 &o)
{
    SetX(GetX()-o.GetX());
    SetY(GetY()-o.GetY());
    return *this;
}

EuclideanVector2 EuclideanVector2::operator - ()
{
    return EuclideanVector2(-GetX(), -GetY());
}
    

END_NAMESPACE_GUTIL;
