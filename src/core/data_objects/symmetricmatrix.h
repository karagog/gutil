/*Copyright 2010-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef SYMMETRICMATRIX_H
#define SYMMETRICMATRIX_H

#include <gutil/imatrix.h>
#include <gutil/vector.h>

NAMESPACE_GUTIL;


/** This class represents a symmetric matrix of items, which optimizes memory
    based on the assumption that across the diagonal all the values are symmetric
*/
template <class T>
class SymmetricMatrix :
        public IMatrix<T>
{
public:

    SymmetricMatrix(GUINT32 size)
        :IMatrix<T>(size, size)
    {
        for(int i = 0; i < size; i++)
            m_matrix.push_back(Vector<T>(i + 1));
    }

    SymmetricMatrix(GUINT32 size, const T &initial_value)
        :IMatrix<T>(size, size)
    {
        for(GUINT32 i = 0; i < size; i++)
        {
            m_matrix.Insert(Vector<T>(i + 1));
            for(GUINT32 j = 0; j < i + 1; j++)
                m_matrix[m_matrix.Length() - 1][j] = initial_value;
        }
    }

    T& Value(int row, int col){
        _transform_indeces(row, col);
        return m_matrix[row][col];
    }
    const T& Value(int row, int col) const{
        _transform_indeces(row, col);
        return m_matrix[row][col];
    }

    T FindMaxValue(int &row, int &col) const;


private:

    Vector< Vector<T> > m_matrix;

    static void _transform_indeces(int &r, int &c){
        // Swap row and column if they indexed the wrong side of the diagonal
        if(r < c)
        {
            const int tmp( r );
            r = c;
            c = tmp;
        }
    }

};



template <class T>
T SymmetricMatrix<T>::FindMaxValue(int &row, int &col) const
{
    if(SymmetricMatrix<T>::RowCount() == 0)
        throw Exception<>("Empty matrix");

    T max = 0;
    bool max_found(false);
    row = -1;  col = -1;

    for(int i = 0; i < SymmetricMatrix<T>::ColumnCount(); i++)
    {
        for(int j = i; j < SymmetricMatrix<T>::RowCount(); j++)
        {
            const T &tmpval( SymmetricMatrix<T>::Value(j, i) );
            if(max_found)
            {
                if(max < tmpval)
                {
                    max = tmpval;
                    row = j; col = i;
                }
            }
            else
            {
                max_found = true;
                max = tmpval;
                row = j; col = i;
            }
        }
    }
    return max;
}


END_NAMESPACE_GUTIL;

#endif // SYMMETRICMATRIX_H
