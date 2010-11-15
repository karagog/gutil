/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef ICOMPARER_H
#define ICOMPARER_H

namespace GUtil
{
    namespace Core
    {
        namespace Interfaces
        {
            // A class to compare two objects of different types
            template <class T, class U> class IComparerDifferent
            {
            public:

                // Return negative if LHS is less than RHS, 0 if equal
                virtual int Compare(const T &lhs, const U &rhs) const = 0;

                // A convenient pointer compare
                virtual inline int Compare(const T* const lhs, const T* const rhs) const
                {
                    return Compare(*lhs, *rhs);
                }

                virtual ~IComparerDifferent(){}
            };



            // A class to compare two objects of like types
            template <class T> class IComparer : public IComparerDifferent<T, T>
            {
            public:

                virtual ~IComparer(){}
            };



            // The default comparer only requires a less-than operator
            template <class T> class DefaultComparer : public IComparer<T>
            {
            public:

                virtual int Compare(const T&lhs, const T& rhs) const
                {
                    int ret = 0;

                    if(lhs < rhs)
                        ret = -1;
                    else if(rhs < lhs)
                        ret = 1;

                    return ret;
                }

                virtual ~DefaultComparer(){}
            };
        }
    }
}

#endif // ICOMPARER_H
