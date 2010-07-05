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

#include "DA_PersistentSettings.h"
#include "DA_UserMachineLock.h"
#include "stringhelpers.h"
#include "exception.h"
#include "qtlockedfile.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDesktopServices>
#include <QCoreApplication>
#include <QDir>
#include <QTimer>
#include <QVariant>
using namespace GUtil;
using namespace GQtUtil::DataAccess;

DA_PersistentSettings::DA_PersistentSettings(const QString &identity)
{
    _userlock = new DA_UserMachineLock(identity);
    _lock_acquired = false;

    _loaded = false;

    load_settings(identity);
}

DA_PersistentSettings::~DA_PersistentSettings()
{
    delete _userlock;
}

bool DA_PersistentSettings::Reload()
{
    return load_settings(_identity_string);
}

bool DA_PersistentSettings::SetValue(const QString &key, const QString& value)
{
    if(!IsLoaded())
    {
        throw GUtil::Exception("You haven't successfully initialized the settings object (Call 'Reload()' until it returns true)");
    }

    _values.insert(key, value);
    return save_settings();
}

bool DA_PersistentSettings::SetValues(const QMap<QString, QString> &values)
{
    if(!IsLoaded())
    {
        throw GUtil::Exception("You haven't successfully initialized the settings object (Call 'Reload()' until it returns true)");
    }

    bool tmp = false;
    foreach(QString s, values.keys())
    {
        _values[s] = values[s];

        if(!tmp)
            tmp = true;
    }

    // Only save settings if we actually made changes
    if(tmp)
        tmp = save_settings();
    return true;
}

QString DA_PersistentSettings::Value(const QString &key)
{
    if(!IsLoaded())
    {
        throw GUtil::Exception("You haven't successfully initialized the settings object (Call 'Reload()' until it returns true)");
    }

    return _values.value(key, "");
}

const QMap<QString, QString> DA_PersistentSettings::Values(const QStringList &keys)
{
    QMap<QString, QString> ret;
    if(!IsLoaded())
    {
        throw GUtil::Exception("You haven't successfully initialized the settings object (Call 'Reload()' until it returns true)");
    }
    else
    {
        foreach(QString s, keys)
        {
            ret[s] = _values.value(s, "");
        }
    }

    return ret;
}

bool DA_PersistentSettings::Contains(const QString &key)
{
    if(!IsLoaded())
    {
        throw GUtil::Exception("You haven't successfully initialized the settings object (Call 'Reload()' until it returns true)");
    }

    return _values.contains(key);
}

bool DA_PersistentSettings::Clear()
{
    _values.clear();

    return save_settings();
}

bool DA_PersistentSettings::Remove(const QString &key)
{
    QMap<QString, QString>::iterator it = _values.find(key);

    // If we didn't find the key then all is well, just skip the erase
    if(it != _values.end())
    {
        _values.erase(it);
        return save_settings();
    }

    return true;
}

bool DA_PersistentSettings::Remove(const QStringList &keys)
{
    QMap<QString, QString>::iterator it;
    bool val_erased = false;

    foreach(QString s, keys)
    {
        it = _values.find(s);

        if(it == _values.end())
            continue;

        _values.erase(it);

        if(!val_erased)
            val_erased = true;
    }

    return val_erased ? save_settings() : true;
}

bool DA_PersistentSettings::save_settings()
{
    if(!IsLoaded())
    {
        throw GUtil::Exception("You haven't successfully initialized the settings object (Call 'Reload()' until it returns true)");
    }

    QString loc;

    loc = get_settings_location();
    Q_ASSERT(loc != "");

    QtLockedFile lf(loc);
    if(!lf.open(QFile::ReadWrite))
    {
        throw GUtil::Exception("Couldn't open the settings file for writing");
    }

    if(!lf.lock(QtLockedFile::WriteLock, true))
    {
        return false;
    }

    if(!lf.resize(0))
        throw GUtil::Exception("Couldn't truncate settings file?");

    // At this point the settings file is ours for sole writing
    QString xmlstr;
    QXmlStreamWriter sw(&xmlstr);
    sw.setAutoFormatting(false);

    sw.writeStartDocument();
    sw.writeStartElement("settings");

    foreach(QString s, _values.keys())
    {
        // Don't bother writing empty settings, because they'll be defaulted to a
        //  null string anyways if they're not found
        QString v = _values[s];
        if(v.length() == 0)
            continue;

        QString encoded_string = QString::fromStdString(
                StringHelpers::toBase64(v.toStdString()));

        sw.writeStartElement(s);
        sw.writeAttribute("v", encoded_string);
        sw.writeEndElement();
    }

    sw.writeEndElement();
    sw.writeEndDocument();

    if(xmlstr.length() != lf.write(xmlstr.toStdString().c_str()))
    {
        lf.close();
        throw GUtil::Exception("Couldn't write all the data to the file");
    }

    lf.unlock();
    lf.close();

    emit NotifySaved();
    return true;
}

bool DA_PersistentSettings::load_settings(const QString &settings_filename)
{
    if(!_lock_acquired && !(_lock_acquired = _userlock->Lock()))
        throw GUtil::Exception("Could not acquire user lock");

    _loaded = false;

    if(settings_filename == "")
    {
        return false;
    }

    QString loc;
    _identity_string = settings_filename;
    loc = get_settings_location();

    QtLockedFile lf(loc);
    if(!lf.open(QFile::ReadOnly))
    {
        throw GUtil::Exception(QString("Couldn't open the settings file for reading: %1")
                               .arg(FileName()).toStdString());
    }

    if(!lf.lock(QtLockedFile::ReadLock, true))
    {
        return false;
    }

    QString dat = QString::fromAscii(lf.readAll());
    _values.clear();

    lf.unlock();
    lf.close();

    QXmlStreamReader sr(dat);
    //Read in the startDocument tag
    sr.readNext();

    if(!sr.readNextStartElement())  //Read in settings root
        throw GUtil::Exception("XML not recognized");

    while(sr.readNextStartElement())
    {
        QString decoded_string = QString::fromStdString(
                StringHelpers::fromBase64(sr.attributes().value("v").toString().toStdString()));
        _values.insert(sr.name().toString(), decoded_string);

        // Read in the end element tag
        sr.readNext();
    }

    return (_loaded = true);
}

QString DA_PersistentSettings::get_settings_location()
{
    QString data_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString app_name = _identity_string;

    QString fl = app_name.toLower() + ".settings.xml";

    QDir tmpdir(data_path);
    Q_ASSERT(tmpdir.exists());

    if(!tmpdir.exists(app_name))
    {
        if(!tmpdir.mkpath(app_name))
            throw GUtil::Exception("Couldn't make settings directory");
    }

    tmpdir.cd(app_name);

    _settings_filename = tmpdir.absolutePath() + "/" + fl;

    if(!tmpdir.exists(fl))
    {
        QFile f(_settings_filename);

        if(!f.open(QFile::WriteOnly))
            throw GUtil::Exception("Couldn't create settings file");

        // An initially parseable xml tree
        f.write("<settings></settings>");

        f.close();
    }

    return _settings_filename;
}

QString DA_PersistentSettings::Error() const
{
    return _error;
}

bool DA_PersistentSettings::IsLoaded() const
{
    return _loaded;
}

QString DA_PersistentSettings::FileName() const
{
    if(!IsLoaded())
    {
        return "";
    }

    return _settings_filename;
}

QString DA_PersistentSettings::Identity() const
{
    return _identity_string;
}

const QString DA_PersistentSettings::operator [](const QString &key) const
{
    return _values[key];
}
