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

BusinessObjects::ConfigFile::ConfigFile(const QString &identifier,
                                   const QString &modifier,
                                   QObject *parent)
    :BusinessObjects::AbstractValueBuffer(new DataAccess::GFileIODevice(
            QString("%1.%2")
            .arg(get_file_location(identifier))
            .arg(modifier)),
                                          parent),
    _p_IsHumanReadable(true),
    _p_AutoCommitChanges(true)
{
    // Set the file transport to overwrite the config file rather than append
    FileTransport().SetWriteMode(DataAccess::GFileIODevice::WriteOver);

    _init(identifier, modifier);
}

BusinessObjects::ConfigFile::ConfigFile(const BusinessObjects::ConfigFile &other, QObject *parent)
    :BusinessObjects::AbstractValueBuffer(new DataAccess::GFileIODevice(other.FileName()), parent),
    _p_IsHumanReadable(other._p_IsHumanReadable),
    _p_AutoCommitChanges(other._p_AutoCommitChanges)
{
    _init(other._identity, other._modifier);
}

void BusinessObjects::ConfigFile::_init(const QString &identity, const QString &modifier)
{
    _identity = identity;
    _modifier = modifier;

    _init_column_headers();

    importData();
}

void BusinessObjects::ConfigFile::Reload()
{
    importData();
}

void BusinessObjects::ConfigFile::Clear()
{
    table().Clear();
    _init_column_headers();

    _value_changed();
}

void BusinessObjects::ConfigFile::_init_column_headers()
{
    // Two columns to the table
    table().SetColumnHeaders(QStringList("key") << "value");
}

void BusinessObjects::ConfigFile::_value_changed()
{
    if(GetAutoCommitChanges())
        CommitChanges();
}



#ifdef CRYPTOPP_COMPRESSION
#   define CRYPTOPP_DECOMPRESS( data ) \
        Core::Utils::CryptoHelpers::decompress( \
                    std::string(data.toStdString().c_str(), data.length()))

#   define CRYPTOPP_COMPRESS( data ) \
        Core::Utils::CryptoHelpers::compress( \
                    std::string(data.toStdString().c_str(), data.length()))
#else
#   define CRYPTOPP_DECOMPRESS(data) std::string(data.toStdString().c_str(), data.length())
#   define CRYPTOPP_COMPRESS(data) std::string(data.toStdString().c_str(), data.length())
#endif

QByteArray BusinessObjects::ConfigFile::get_current_data() const
{
    QByteArray ba = AbstractValueBuffer::get_current_data(GetIsHumanReadable());

    #ifdef CRYPTOPP_COMPRESSION
    if(!IsHumanReadable())
    {
        // Compress the configuration data
        std::string tmpres = CRYPTOPP_COMPRESS(ba);
        ba = QByteArray(tmpres.c_str(), tmpres.length());
    }
    #endif

    return ba;
}

QString BusinessObjects::ConfigFile::import_incoming_data()
{
    QString data = AbstractValueBuffer::import_incoming_data();


    #ifdef CRYPTOPP_COMPRESSION
    // try to decompress it,
    try
    {
        std::string tmpres = CRYPTOPP_DECOMPRESS(data);

        data = QString(tmpres.c_str());
    }
    catch(Core::Exception &)
    {}
    #endif

    return data;
}

void BusinessObjects::ConfigFile::SetValue(const QString &key, const Custom::GVariant& value)
{
    QMap<QString, Custom::GVariant> m;
    m.insert(key, value);
    return SetValues(m);
}

void BusinessObjects::ConfigFile::SetValues(const QMap<QString, Custom::GVariant> &values)
{
    if(values.keys().count() == 0)
        return;

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

    _value_changed();
}

Custom::GVariant BusinessObjects::ConfigFile::Value(const QString &key) const
{
    return Values(QStringList(key)).value(key);
}

QMap<QString, Custom::GVariant> BusinessObjects::ConfigFile::Values(const QStringList &keys) const
{
    QMap<QString, Custom::GVariant> ret;
    QStringList keys_copy(keys);

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

    return ret;
}

bool BusinessObjects::ConfigFile::Contains(const QString &key) const
{
    bool ret = true;
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

    return ret;
}

void BusinessObjects::ConfigFile::RemoveValue(const QString &key)
{
    QStringList sl;
    sl.append(key);
    RemoveValues(sl);
}

void BusinessObjects::ConfigFile::RemoveValues(const QStringList &keys)
{
    if(keys.count() == 0)
        return;

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

    _value_changed();
}

QString BusinessObjects::ConfigFile::get_file_location(QString id)
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

void BusinessObjects::ConfigFile::process_input_data(const QByteArray &ba)
{
    AbstractValueBuffer::process_input_data(ba);

    // copy the input data to the current data table
    table() = table_incoming().Clone();

    emit NotifyConfigurationUpdate();
}

void BusinessObjects::ConfigFile::commit_reject_changes(bool commit)
{
    table().CommitChanges(commit);

    if(commit)
        // Export the changed data to the config file (don't clear the current data in it)
        enQueueCurrentData(false);
}
