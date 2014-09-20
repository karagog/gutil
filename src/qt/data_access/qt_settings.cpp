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
#include "gutil_settings.h"
#include "gutil_variant.h"
#include <QDesktopServices>
#include <QDir>
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(QT);

static QString __get_filename(const QString &identity, const QString &modifier)
{
    QString data_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    QDir tmpdir(data_path);
    if(!tmpdir.exists()){
        if(!tmpdir.mkpath(data_path))
            return QString::null;
    }

    QString ret = QString("%1/%2")
            .arg(tmpdir.absolutePath())
            .arg(identity);
    if(!modifier.isEmpty())
        ret.append(".").append(modifier);
    ret.append(".GUtilSettings");
    return ret;
}


Settings::Settings(const char *identity, const char *modifier)
    :m_identity(identity),
      m_modifier(modifier),
      m_filename(__get_filename(m_identity, m_modifier)),
      m_settings(new GUtil::Settings(m_filename.toUtf8()))
{}

Settings::~Settings() {}

StringList Settings::Keys() const
{
    return m_settings->Keys();
}

QVariant Settings::Value(const String &key) const
{
    return Variant::ConvertFromXmlQString(m_settings->GetValue(key).ToQString());
}

void Settings::SetValue(const String &key, const QVariant &value)
{
    String data = Variant::ConvertToXmlQString(value);
    m_settings->SetValue(key, data);
}

void Settings::RemoveValue(const String &key)
{
    m_settings->Remove(key);
}

bool Settings::Contains(const String &key) const
{
    return m_settings->Contains(key);
}

bool Settings::IsDirty() const
{
    return m_settings->IsDirty();
}

void Settings::CommitChanges()
{
    m_settings->CommitChanges();
}


END_NAMESPACE_GUTIL1;
