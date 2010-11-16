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

#include "Core/exception.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include "Core/Interfaces/iequatable.h"
#include "Core/Interfaces/iclonable.h"
#include <QList>

namespace GUtil
{
    namespace DataObjects
    {
        template <typename T> class CollectionBase :
                public Core::Interfaces::IReadOnlyObject,
                protected Core::Interfaces::IEquatable< CollectionBase<T> >,
                protected Core::Interfaces::IClonable< CollectionBase<T> >
        {
        public:

            T &operator [](int index)
                    throw(Core::IndexOutOfRangeException);


        protected:

            // Derived classes can do things to the added/removed object by overriding
            virtual void on_add(T *) const{}
            virtual void on_remove(T *) const{}

            // You can define a different equality specification by overriding
            virtual bool compare_equality(const T &lhs, const T &rhs) const{
                return lhs == rhs;
            }

            // Derived classes use this method to instantiate blank items for the resize method
            virtual T create_blank_item(){
                THROW_NEW_GUTIL_EXCEPTION(Core::NotImplementedException,
                                          "Derived class must override this method!");
            }

            // Derived classes use this to reject new items for any reason
            virtual void validate_new_item(const T &) const
                    throw(Core::ValidationException){}


            CollectionBase(int size = 0);
            CollectionBase(const CollectionBase<T> &);
            CollectionBase(const QList<T> &);
            virtual ~CollectionBase();

            T &add_protected(const T &value);
            T &insert_protected(int index, const T &value)
                throw(Core::IndexOutOfRangeException);

            T &setValue_protected(int index, const T &)
                throw(Core::IndexOutOfRangeException);
            T value_protected(int index) const
                    throw(Core::IndexOutOfRangeException);

            void remove_protected(int index)
                    throw(Core::IndexOutOfRangeException);
            void removeOne_protected(const T &);
            void removeAll_protected(const T &);
            void clear_protected();

            int count_protected() const;
            int size_protected() const;
            void resize_protected(int);

            bool contains_protected(const T &) const;
            int indexOf_protected(const T &) const;


            // The IEquatable interface:
            virtual bool Equals(const CollectionBase<T> &) const;

            // The IClonable interface
            virtual CollectionBase<T> &CloneTo(CollectionBase<T> &) const;


        private:

            QList<T> _collection;

        };



        template <typename T> class Collection : public CollectionBase<T>
        {
        public:

            Collection(int size = 0);
            Collection(const Collection<T> &);
            Collection(const QList<T> &);
            virtual ~Collection();


            T &Add(const T &value);
            T &Insert(int index, const T &value)
                throw(Core::IndexOutOfRangeException);

            T &SetValue(int index, const T &)
                throw(Core::IndexOutOfRangeException);
            T Value(int index) const
                    throw(Core::IndexOutOfRangeException);

            void Remove(int index)
                    throw(Core::IndexOutOfRangeException);
            void RemoveOne(const T &);
            void RemoveAll(const T &);
            void Clear();

            int Count() const;
            int Size() const;
            void Resize(int);

            bool Contains(const T &) const;
            int IndexOf(const T &) const;


            // The IEquatable interface:
            virtual bool Equals(const Collection<T> &) const;

            // The IClonable interface
            virtual Collection<T> &CloneTo(Collection<T> &) const;

        };
    }
}

#include "collection.cpp"

#endif // Collection_H
