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
#include "DataTransports/filetransport.h"
#include "Core/exception.h"
#include "Core/Tools/stringhelpers.h"
#include "qtlockedfile.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileSystemWatcher>
#include <QDesktopServices>
#include <QDir>
#include <QTimer>
using namespace GUtil;

DataAccess::ConfigFile::ConfigFile(const QString &identifier,
                                   const QString &modifier,
                                   Utils::AbstractLogger *logger,
                                   QObject *parent)
    :DataAccess::AbstractValueBuffer(new DataAccess::DataTransports::FileTransport(
            QString("%1.%2")
            .arg(get_file_location(_identity))
            .arg(_modifier)),
                                     logger,
                                     parent)
{
    _init(identifier, modifier);
}

DataAccess::ConfigFile::ConfigFile(const DataAccess::ConfigFile &other, QObject *parent)
    :AbstractValueBuffer(new DataAccess::DataTransports::FileTransport(other.fileName()),
                         other.Logger(), parent)
{
    _init(other._identity, other._modifier);
}

DataAccess::ConfigFile::~ConfigFile()
{
}

void DataAccess::ConfigFile::_init(const QString &identity, const QString &modifier)
{
    _identity = identity;
    _modifier = modifier;

    importData(_file_transport->fileData());

    connect(_file_transport, SIGNAL(notifyNewData(QByteArray)),
            this, SLOT(catch_asynchronous_update(QByteArray)));
}

QString DataAccess::ConfigFile::fileName() const
{
    return _file_transport->fileName();
}

void DataAccess::ConfigFile::reload()
{
    _file_transport->reload();
}

void DataAccess::ConfigFile::getIdentity(QString &identifier, QString &modifier)
{
    identifier = _identity;
    modifier = _modifier;
}

void DataAccess::ConfigFile::ValueChanged_protected()
{
    // Export the changed data to the config file
    throw Core::NotImplementedException();
    //enQueue(true);
    //exportData();
}

void DataAccess::ConfigFile::process_input_data(const QByteArray &)
{

}

DataAccess::DataTransports::FileTransport *DataAccess::ConfigFile::FileTransport() const
{
    return (DataAccess::DataTransports::FileTransport *)Transport();
}

QString DataAccess::ConfigFile::get_file_location(QString id)
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

void DataAccess::ConfigFile::catch_asynchronous_update(const QByteArray &dat)
{
    importData(dat);

    emit notifyConfigurationUpdate();
}

std::string DataAccess::ConfigFile::ReadonlyMessageIdentifier() const
{
    return "DataAccess::ConfigFile";
}
