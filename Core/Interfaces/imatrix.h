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

#ifndef IMATRIX_H
#define IMATRIX_H

#include "gutil_macros.h"

GUTIL_BEGIN_CORE_NAMESPACE( Interfaces );


template <class T> class IMatrix
{
public:

    IMatrix(int rows = 0, int cols = 0)
        :m_rows(rows),
          m_cols(cols)
    {}
    virtual ~IMatrix(){}

    inline int RowCount() const{ return m_rows; }
    inline int ColumnCount() const{ return m_cols; }

    // Derived classes implement the accessor functions
    virtual T &Value(int row, int col) = 0;
    virtual const T &Value(int row, int col) const = 0;


protected:

    // Derived classes have direct access to the row/column memory
    int m_rows, m_cols;

};


GUTIL_END_CORE_NAMESPACE;

#endif // IMATRIX_H
