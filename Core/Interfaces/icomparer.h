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
                virtual bool Compare(const T &, const U &) const = 0;
            };



            // A class to compare two objects of like types
            template <class T> class IComparer : public IComparerDifferent<T, T>
            {};
        }
    }
}

#endif // ICOMPARER_H
