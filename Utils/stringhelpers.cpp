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

#include "stringhelpers.h"
#include "Core/Utils/stringhelpers.h"
#include <QString>
using namespace GUtil;

QString Utils::StringHelpers::toBase64(const QString &s)
{
    return QString::fromStdString(
            Core::Utils::StringHelpers::toBase64(
                    s.toStdString()));
}

QString Utils::StringHelpers::fromBase64(const QString &s)
{
    return QString::fromStdString(
            Core::Utils::StringHelpers::fromBase64(
                    s.toStdString()));
}

QString Utils::StringHelpers::toBase16(const QString &s)
{
    return QString::fromStdString(
            Core::Utils::StringHelpers::toBase16(
                    s.toStdString()));
}

QString Utils::StringHelpers::fromBase16(const QString &s)
{
    return QString::fromStdString(
            Core::Utils::StringHelpers::fromBase16(
                    s.toStdString()));
}