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

#ifndef DATACOLUMN_H
#define DATACOLUMN_H

#include "Core/Interfaces/iequatable.h"
#include <QPair>

namespace GUtil
{
    namespace DataObjects
    {
        class DataColumn : public Core::Interfaces::IEquatable<DataColumn>
        {
        public:

            QString Key() const;
            QString Label() const;

            DataColumn &operator =(const DataColumn &);

            // IEquatable:
            virtual bool Equals(const DataColumn &) const;


        protected:

            DataColumn(const QString &key);
            DataColumn(const QPair<QString, QString> &);
            DataColumn(const DataColumn &);


        private:

            QPair<QString, QString> _data;

        };
    }
}

#endif // DATACOLUMN_H
