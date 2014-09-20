/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "qt_settings.h"
#include "gutil_variant.h"
#include <QDesktopServices>
#include <QDir>
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(QT);

static String __get_filename(const char *identity, const char *modifier)
{
    QString data_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    QDir tmpdir(data_path);
    if(!tmpdir.exists()){
        if(!tmpdir.mkpath(data_path))
            return String();
    }

    String ret = String::Format("%s/%s",
                                tmpdir.absolutePath().toUtf8().constData(),
                                identity);
    if(modifier && strlen(modifier) > 0)
        ret.Append(".").Append(modifier);
    return ret.Append(".GUtilSettings");
}


Settings::Settings(const char *identity, const char *modifier, QObject *parent)
    :QObject(parent),
      GUtil::Settings(__get_filename(identity, modifier)),
      m_identity(identity),
      m_modifier(modifier)
{}

StringList Settings::Keys()
{
    return GUtil::Settings::Keys();
}

QVariant Settings::Value(const String &key)
{
    return Variant::ConvertFromXmlQString(GUtil::Settings::GetValue(key).ToQString());
}

void Settings::SetValue(const String &key, const QVariant &value)
{
    String data = Variant::ConvertToXmlQString(value);
    GUtil::Settings::SetValue(key, data);
}

void Settings::RemoveValue(const String &key)
{
    GUtil::Settings::Remove(key);
}

bool Settings::Contains(const String &key)
{
    return GUtil::Settings::Contains(key);
}

bool Settings::IsDirty()
{
    return GUtil::Settings::IsDirty();
}

void Settings::CommitChanges()
{
    GUtil::Settings::CommitChanges();
}


END_NAMESPACE_GUTIL1;
