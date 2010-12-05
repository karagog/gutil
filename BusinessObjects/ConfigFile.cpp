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
#include "DataAccess/gfileiodevice.h"
#include "Core/exception.h"
#include "Core/Utils/stringhelpers.h"
#include "Core/Utils/encryption.h"
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
    _config_is_human_readable(true)
{
    // Set the file transport to overwrite the config file rather than append
    FileTransport().SetWriteMode(DataAccess::GFileIODevice::WriteOver);

    _init(identifier, modifier);
}

BusinessObjects::ConfigFile::ConfigFile(const BusinessObjects::ConfigFile &other, QObject *parent)
    :BusinessObjects::AbstractValueBuffer(new DataAccess::GFileIODevice(other.FileName()), parent)
{
    _init(other._identity, other._modifier);
}

void BusinessObjects::ConfigFile::_init(const QString &identity, const QString &modifier)
{
    SetHumanReadable(false);

    _identity = identity;
    _modifier = modifier;

    importData();
}

void BusinessObjects::ConfigFile::Reload()
{
    importData();
}

QString BusinessObjects::ConfigFile::FileName() const
{
    return FileTransport().FileName();
}

void BusinessObjects::ConfigFile::GetIdentity(QString &identifier, QString &modifier)
{
    identifier = _identity;
    modifier = _modifier;
}

void BusinessObjects::ConfigFile::ValueChanged_protected() throw(Core::Exception)
{
    // Export the changed data to the config file
    enQueueCurrentData(false);
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
    QByteArray ba = AbstractValueBuffer::get_current_data(IsHumanReadable());

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

QString BusinessObjects::ConfigFile::import_current_data()
{
    QString data = AbstractValueBuffer::import_current_data();

    #ifdef CRYPTOPP_COMPRESSION
    // try to decompress it,
    try
    {
        std::string tmpres = CRYPTOPP_DECOMPRESS(data);

        data = QString(tmpres.c_str());
    }
    catch(Core::Exception)
    {}
    #endif

    return data;
}

DataAccess::GFileIODevice &BusinessObjects::ConfigFile::FileTransport() const
{
    return (DataAccess::GFileIODevice &)Transport();
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

std::string BusinessObjects::ConfigFile::ReadonlyMessageIdentifier() const
{
    return "DataAccess::ConfigFile";
}

bool BusinessObjects::ConfigFile::IsHumanReadable() const
{
    return _config_is_human_readable;
}

void BusinessObjects::ConfigFile::SetHumanReadable(bool r)
{
    _config_is_human_readable = r;
}
