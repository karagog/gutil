/*Copyright 2010-2012 George Karagoulis

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

#include "gutil_exception.h"

NAMESPACE_GUTIL1( Interfaces );


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

    T FindMaxInRow(int row, int &found_indexex) const;
    T FindMinInRow(int row, int &found_indexex) const;
    T FindMaxInCol(int col, int &row_index) const;
    T FindMinInCol(int col, int &row_index) const;

    virtual T FindMaxValue(int &row, int &col) const;


protected:

    // Derived classes have direct access to the row/column memory
    int m_rows, m_cols;


private:

    T _find_optimum_in_row(int,
                           int &,
                           bool search_row,
                           bool (*compare)(const T &, const T &)) const;

    static bool normal_compare(const T &lhs, const T &rhs){
        return lhs < rhs;
    }
    static bool reverse_compare(const T&lhs, const T &rhs){
        return rhs < lhs;
    }

};



template <class T>
T IMatrix<T>::FindMaxInRow(int index, int &found_index) const
{
    return _find_optimum_in_row(index, found_index, true, &normal_compare);
}
template <class T>
T IMatrix<T>::FindMinInRow(int index, int &found_index) const
{
    return _find_optimum_in_row(index, found_index, true, &reverse_compare);
}
template <class T>
T IMatrix<T>::FindMaxInCol(int index, int &found_index) const
{
    return _find_optimum_in_row(index, found_index, false, &normal_compare);
}
template <class T>
T IMatrix<T>::FindMinInCol(int index, int &found_index) const
{
    return _find_optimum_in_row(index, found_index, false, &reverse_compare);
}
template <class T>
T IMatrix<T>::FindMaxValue(int &row, int &col) const
{
    if(RowCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "Empty matrix");

    T max;
    bool max_found(false);
    row = -1;  col = -1;

    for(int i = 0; i < RowCount(); i++)
    {
        int tmpcol(-1);
        const T tmpval(_find_optimum_in_row(i, tmpcol, true, &normal_compare));
        if(tmpcol != -1)
        {
            if(max_found)
            {
                if(normal_compare(max, tmpval))
                {
                    max = tmpval;
                    row = i; col = tmpcol;
                }
            }
            else
            {
                max_found = true;
                max = tmpval;
                row = i; col = tmpcol;
            }
        }
    }
    return max;
}

template <class T>
T IMatrix<T>::_find_optimum_in_row(int index,
                                   int &found_index,
                                   bool search_row,
                                   bool (*compare)(const T &, const T &)) const
{
    T max;
    bool max_found(false);
    int max_index(-1);

    const int lim( search_row ? ColumnCount() : RowCount() );

    for(int i = 0; i < lim; i++)
    {
        const int rowInd(search_row ? index : i);
        const int colInd(search_row ? i : index);

        if(max_found)
        {
            const T &tmpval( Value(rowInd, colInd) );
            if(compare(max, tmpval))
            {
                max = tmpval;
                max_index = i;
            }
        }
        else
        {
            max = Value(rowInd, colInd);
            max_index = i;
            max_found = true;
        }
    }

    found_index = max_index;
    return max;
}


END_NAMESPACE_GUTIL1;

#endif // IMATRIX_H
