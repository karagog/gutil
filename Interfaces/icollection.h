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

#ifndef ICOLLECTION_H
#define ICOLLECTION_H

#include "Core/Interfaces/ireadonlyobject.h"
#include <QList>

namespace GUtil
{
    namespace Interfaces
    {
        template <typename T> class ICollection : public Core::Interfaces::IReadOnlyObject
        {
        public:
            ICollection(int size = 0);
            ICollection(const ICollection<T> &o);
            virtual ~ICollection();

            T &Add(const T &value);
            T &Insert(int index, const T &value);

            T &SetValue(int index, const T &);
            T Value(int index) const;

            T &operator [](int index);
            virtual ICollection<T> &operator =(const ICollection<T> &);

            void Remove(int index);
            void ClearValues();

            int Count() const;
            int Size() const;
            void Resize(int);

        protected:

            virtual void onAdd(T &);

        private:
            QList<T> _collection;

            static void _copy(ICollection<T> &, const ICollection<T> &);
        };
    }
}

#include "icollection.cpp"

#endif // ICOLLECTION_H
