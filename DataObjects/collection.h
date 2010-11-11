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

#ifndef COLLECTION_H
#define COLLECTION_H

#include "Core/Interfaces/ireadonlyobject.h"
#include "Core/Interfaces/iequatable.h"
#include "Core/Interfaces/iclonable.h"
#include <QList>

namespace GUtil
{
    namespace DataObjects
    {
        template <typename T> class Collection :    public Core::Interfaces::IReadOnlyObject,
                                                    public Core::Interfaces::IEquatable< Collection<T> >
        {
        public:
            Collection(int size = 0);
            Collection(const Collection<T> &);
            Collection(const QList<T> &);
            virtual ~Collection();

            T &Add(const T &value);
            T &Insert(int index, const T &value);

            T &SetValue(int index, const T &);
            T Value(int index) const;

            virtual T &operator [](int index);
            virtual Collection<T> &operator =(const Collection<T> &);

            void Remove(int index);
            void Clear();

            int Count() const;
            int Size() const;
            void Resize(int);

            bool Contains(const T &) const;
            int IndexOf(const T &) const;

            bool operator ==(const Collection<T> &) const;
            virtual bool Equals(const Collection<T> &) const;

            virtual Collection<T> &CloneOnto(Collection<T> &) const;

        protected:

            virtual void onAdd(T &);

        private:
            QList<T> _collection;

        };
    }
}

#include "collection.cpp"

#endif // Collection_H
