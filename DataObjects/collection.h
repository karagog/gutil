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
                                                    public Core::Interfaces::IEquatable< Collection<T> >,
                                                    public Core::Interfaces::IClonable< Collection<T> >
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

            T &operator [](int index);

            void Remove(int index);
            void RemoveOne(const T &);
            void RemoveAll(const T &);
            void Clear();

            int Count() const;
            int Size() const;
            void Resize(int);

            bool Contains(const T &) const;
            int IndexOf(const T &) const;

            bool operator ==(const Collection<T> &) const;
            bool operator !=(const Collection<T> &) const;

            // The IEquatable interface:
            virtual bool Equals(const Collection<T> &) const;

            // The IClonable interface
            virtual Collection<T> &CloneTo(Collection<T> &) const;


        protected:

            // Derived classes can do things to the added/removed object by overriding
            virtual void on_add(T *) const{}
            virtual void on_remove(T *) const{}

            // You can define a different equality specification by overriding
            virtual bool compare_equality(const T &lhs, const T &rhs) const{
                return lhs == rhs;
            }

            // Derived classes use this method to instantiate blank items for the resize method
            virtual T create_blank_item() const{
                THROW_NEW_GUTIL_EXCEPTION(Core::NotImplementedException,
                                          "Derived class must override this method!");
            }

            // Derived classes use this to reject new items for any reason
            virtual void validate_new_item(const T &) const
                    throw(Core::ValidationException){}


        private:

            QList<T> _collection;

        };
    }
}

#include "collection.cpp"

#endif // Collection_H
