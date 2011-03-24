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

#ifndef SYMMETRICMATRIX_H
#define SYMMETRICMATRIX_H

#include "Core/Interfaces/imatrix.h"
#include <vector>

GUTIL_BEGIN_CORE_NAMESPACE( Utils );


// This class represents a symmetric matrix of items, which optimizes memory
//  based on the assumption that across the diagonal all the values are symmetric
template <class T> class SymmetricMatrix :
        public Interfaces::IMatrix<T>
{
public:

    SymmetricMatrix(int size)
        :Interfaces::IMatrix<T>(size, size)
    {
        for(int i = 0; i < size; i++)
            m_matrix.push_back(std::vector<T>(i + 1));
    }
    SymmetricMatrix(int size, const T &initial_value)
        :Interfaces::IMatrix<T>(size, size)
    {
        for(int i = 0; i < size; i++)
        {
            m_matrix.push_back(std::vector<T>(i + 1));
            for(int j = 0; j < i + 1; j++)
                m_matrix.back().at(j) = initial_value;
        }
    }

    T& Value(int row, int col){
        _transform_indeces(row, col);
        return m_matrix.at(row).at(col);
    }
    const T& Value(int row, int col) const{
        _transform_indeces(row, col);
        return m_matrix.at(row).at(col);
    }

    // Override this function to optimize the search for our symmetry
    T FindMaxValue(int &row, int &col) const;


private:

    std::vector< std::vector<T> > m_matrix;

    inline static void _transform_indeces(int &r, int &c){
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
    T max;
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


GUTIL_END_CORE_NAMESPACE;

#endif // SYMMETRICMATRIX_H
