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

#ifndef QVARIANTCOLLECTION_H
#define QVARIANTCOLLECTION_H

#include "gutil_macros.h"
#include <QVariant>

GUTIL_BEGIN_NAMESPACE( DataObjects );


template <class T> class ResizableCollection;

// A collection of QVariants, built on top of our Collection class
typedef ResizableCollection<QVariant> QVariantCollection;


GUTIL_END_NAMESPACE

#endif // QVARIANTCOLLECTION_H
