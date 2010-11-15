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

#include "columncollection.h"
using namespace GUtil;

QString DataObjects::ColumnCollection::Key(int ind) const
{
    return Value(ind).first;
}

QString DataObjects::ColumnCollection::Label(int ind) const
{
    return Value(ind).second;
}

QPair<QString, QString> DataObjects::ColumnCollection::create_blank_item() const
{
    return QPair<QString, QString>();
}

void DataObjects::ColumnCollection::validate_new_item(const QPair<QString, QString> &) const
        throw(Core::ValidationException)
{

}
