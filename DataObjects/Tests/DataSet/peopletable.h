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

#ifndef PEOPLETABLE_H
#define PEOPLETABLE_H

#include "DataObjects/datatable.h"
#include "DataObjects/dataset.h"

class PersonDataRow;

typedef GUtil::DataObjects::DataTableBase<PersonDataRow> PeopleTable;

#endif // PEOPLETABLE_H
