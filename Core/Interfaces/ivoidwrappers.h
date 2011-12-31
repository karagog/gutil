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

#ifndef GUTIL_IVOIDWRAPPER_H
#define GUTIL_IVOIDWRAPPER_H

#include "gutil_macros.h"
NAMESPACE_GUTIL1(Interfaces);


/** \file

    A collection of interfaces that provide various black-box style functionalities dealing
    with void pointers.  This gives base classes a way of abstracting away type-dependent
    data and functions.
*/


/** An interface that declares a comparison for void pointers.
    \note This does not derive from IComparer, to allow derived classes to
    implement that interface with their typed values.
*/
class IVoidComparer
{

public:
    /** A wrapper function to comply with the IComparer interface.
        It reinterprets the void*'s as their typed values and calls compare_typed_values()
        \sa compare_typed_values
    */
    virtual int CompareVoid(const void *const lhs, const void *const rhs) const = 0;

    /** So you can be deleted by this interface. */
    virtual ~IVoidComparer(){}
};


/** A factory class that allocates memory in a black-box style.
    \sa IVoidNewer IVoidDeleter
*/
class IVoidCopyer
{
public:
    /** Creates a copy of the given data object. */
    virtual void *CopyVoid(const void *const v) const = 0;

    /** So you can be deleted by this interface. */
    virtual ~IVoidCopyer(){}
};


/** Allocates memory for a new object.
    \sa IVoidCopyer IVoidDeleter
*/
class IVoidNewer
{
public:
    virtual void *NewVoid() const = 0;

    /** So you can be deleted by this interface. */
    virtual ~IVoidNewer(){}
};


/** De-allocates memory held in the void pointer.
    \sa IVoidNewer IVoidCopyer
*/
class IVoidDeleter
{
public:
    virtual void DeleteVoid(void *) const = 0;

    /** So you can be deleted by this interface. */
    virtual ~IVoidDeleter(){}
};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_IVOIDWRAPPER_H
