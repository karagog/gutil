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

            void Add(const T &value);
            void Insert(int index, const T &value);

            void SetValue(int index, const T &);
            T Value(int index) const;

            T &operator [](int index);
            ICollection<T> &operator =(const ICollection<T> &);

            void Remove(int index);
            void ClearValues();

            int Count() const;
            int Size() const;
            void Resize(int);

        protected:

            // the void * points to the object added (type T)
            virtual void onAdd(void *, int index);

        private:
            QList<T> _collection;
        };
    }
}

#include "icollection.cpp"

#endif // ICOLLECTION_H
