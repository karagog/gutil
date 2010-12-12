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

#include "ConfigFile.h"
#include "Core/exception.h"
#include "Core/Utils/stringhelpers.h"
#include "Core/Utils/encryption.h"
#include "ThirdParty/QtLockedFile/qtlockedfile.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileSystemWatcher>
#include <QDesktopServices>
#include <QDir>
#include <QTimer>
using namespace GUtil;
using namespace BusinessObjects;

ConfigFile::ConfigFile(const QString &identifier,
                                   const QString &modifier,
                                   QObject *parent)
    :AbstractValueBuffer(
            new DataAccess::GFileIODevice(QString("%1.%2")
                                          .arg(get_file_location(identifier))
                                          .arg(modifier)),
            this,
            parent),
    _p_IsHumanReadable(true),
    _p_AutoCommitChanges(true)
{
    // Set the file transport to overwrite the config file rather than append
    FileTransport().SetWriteMode(DataAccess::GFileIODevice::WriteOver);

    _init(identifier, modifier);
}

ConfigFile::ConfigFile(const BusinessObjects::ConfigFile &other,
                       QObject *parent)
    :AbstractValueBuffer(
            new DataAccess::GFileIODevice(other.FileName()),
            this,
            parent),
    _p_IsHumanReadable(other._p_IsHumanReadable),
    _p_AutoCommitChanges(other._p_AutoCommitChanges)
{
    _init(other._identity, other._modifier);
}

void ConfigFile::_init(const QString &identity, const QString &modifier)
{
    _identity = identity;
    _modifier = modifier;

    table_lock().lock();
    {
        _init_column_headers();
    }
    table_lock().unlock();

    importData();
}

void ConfigFile::Reload()
{
    importData();
}

void ConfigFile::Clear()
{
    table_lock().lock();
    {
        table().Clear();
        _init_column_headers();
    }
    table_lock().unlock();

    _value_changed();
}

void ConfigFile::_init_column_headers()
{
    // Two columns to the table
    table().SetColumnHeaders(QStringList("key") << "value");
}

void ConfigFile::_value_changed()
{
    if(GetAutoCommitChanges())
        CommitChanges();
}



#ifdef CRYPTOPP_COMPRESSION
#   define CRYPTOPP_DECOMPRESS( data ) \
        Core::Utils::CryptoHelpers::decompress( \
                    std::string(data.constData(), data.length()))

#   define CRYPTOPP_COMPRESS( data ) \
        Core::Utils::CryptoHelpers::compress( \
                    std::string(data.constData(), data.length()))
#else
#   define CRYPTOPP_DECOMPRESS(data) \
        std::string(data.constData(), data.length())
#   define CRYPTOPP_COMPRESS(data) \
        std::string(data.constData(), data.length())
#endif

void ConfigFile::preprocess_outgoing_data(QByteArray &ba) const
{
    #ifdef CRYPTOPP_COMPRESSION
    if(!IsHumanReadable())
    {
        // Compress the configuration data
        std::string tmpres = CRYPTOPP_COMPRESS(ba);
        ba = QByteArray(tmpres.c_str(), tmpres.length());
    }
    #endif
}

void ConfigFile::preprocess_incoming_data(QByteArray &data) const
{
    #ifdef CRYPTOPP_COMPRESSION
    // try to decompress it,
    try
    {
        data = CRYPTOPP_DECOMPRESS(data).c_str();
    }
    catch(Core::Exception &)
    {}
    #endif
}

void ConfigFile::new_input_data_arrived(const DataObjects::DataTable &tbl)
{
    // copy the input data to the current data table
    table_lock().lock();
    {
        table() = tbl.Clone();
    }
    table_lock().unlock();

    emit NotifyConfigurationUpdate();
}

void ConfigFile::SetValue(const QString &key, const Custom::GVariant& value)
{
    QMap<QString, Custom::GVariant> m;
    m.insert(key, value);
    return SetValues(m);
}

void ConfigFile::SetValues(const QMap<QString, Custom::GVariant> &values)
{
    if(values.keys().count() == 0)
        return;

    table_lock().lock();
    foreach(QString s, values.keys())
    {
        bool ex_hit = false;
        try
        {
            DataObjects::DataRow &r = table().FindFirstRow(0, s);
            r[1] = values[s];
        }
        catch(Core::NotFoundException &)
        {
            ex_hit = true;
        }

        if(ex_hit)
            table().AddNewRow(Custom::GVariantList() << s << values[s]);
    }
    table_lock().unlock();

    _value_changed();
}

Custom::GVariant ConfigFile::Value(const QString &key)
{
    return Values(QStringList(key)).value(key);
}

QMap<QString, Custom::GVariant> BusinessObjects::ConfigFile::Values(
        const QStringList &keys)
{
    QMap<QString, Custom::GVariant> ret;
    QStringList keys_copy(keys);

    table_lock().lock();
    {
        if(keys.isEmpty())
        {
            for(int i = 0; i < table().RowCount(); i++)
                keys_copy.append(table()[i]["key"].toString());
        }

        foreach(QString s, keys_copy)
        {
            // Prepare the "search map"
            QMap<int, Custom::GVariant> m;
            m.insert(0, s);

            try
            {
                const DataObjects::DataRow *const r = &table().FindFirstRow(m);
                ret.insert(r->At(0).toString(), r->At(1));
            }
            catch(Core::NotFoundException &){}    // if key not found, ignore
        }
    }
    table_lock().unlock();

    return ret;
}

bool ConfigFile::Contains(const QString &key)
{
    bool ret = true;

    table_lock().lock();
    try
    {
        QMap<int, Custom::GVariant> m;
        m.insert(0, key);
        table().FindFirstRow(m);
    }
    catch(Core::NotFoundException &)
    {
        ret = false;
    }
    table_lock().unlock();

    return ret;
}

void ConfigFile::RemoveValue(const QString &key)
{
    QStringList sl;
    sl.append(key);
    RemoveValues(sl);
}

void ConfigFile::RemoveValues(const QStringList &keys)
{
    if(keys.count() == 0)
        return;

    table_lock().lock();
    foreach(QString s, keys)
    {
        try
        {
            table().RemoveRow(
                    table().FindFirstRow(0, s)
                    );
        }
        catch(Core::NotFoundException &){}
    }
    table_lock().unlock();

    _value_changed();
}

QString ConfigFile::get_file_location(QString id)
{
    QString data_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    QString fl = id.toLower() + ".config.xml";

    QDir tmpdir(data_path);
    Q_ASSERT(tmpdir.exists());

    if(!tmpdir.exists(id))
    {
        if(!tmpdir.mkpath(id))
            return QString::null;
    }

    tmpdir.cd(id);

    QString _config_filename = QString("%1/%2").arg(tmpdir.absolutePath()).arg(fl);

    if(!tmpdir.exists(fl))
    {
        QFile f(_config_filename);

        if(!f.open(QFile::WriteOnly))
            return QString::null;

        // An initially parseable xml tree
        f.write("<config></config>");

        f.close();
    }

    return _config_filename;
}

void ConfigFile::commit_reject_changes(bool commit)
{
    if(commit)
        // Export the changed data to the config file
        //  (don't clear the current data in it)
        enQueueCurrentData(false);
}
