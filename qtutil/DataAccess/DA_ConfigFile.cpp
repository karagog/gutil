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

#include "DA_ConfigFile.h"
#include "Private/Transports/filetransport.h"
#include "exception.h"
#include "stringhelpers.h"
#include "qtlockedfile.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileSystemWatcher>
#include <QDesktopServices>
#include <QDir>
#include <QTimer>
using namespace GUtil;
using namespace GQtUtil::DataAccess;
using namespace GQtUtil::DataAccess::Private;

DA_ConfigFile::DA_ConfigFile(const QString &identifier, const QString &modifier, QObject *parent)
    :ValueBuffer(new FileTransport(QString("%1.%2")
                                   .arg(get_file_location(identifier))
                                   .arg(modifier)),
                 parent)
{
    _init(identifier, modifier);
}

DA_ConfigFile::DA_ConfigFile(const DA_ConfigFile &other, QObject *parent)
    :ValueBuffer(new FileTransport(QString("%1.%2")
                                   .arg(get_file_location(other._identity))
                                   .arg(other._modifier)),
                 parent)
{
    _init(other._identity, other._modifier);
}

void DA_ConfigFile::_init(const QString &identity, const QString &modifier)
{
    _identity = identity;
    _modifier = modifier;

    importData(get_file_transport()->fileData());

    connect(get_file_transport(), SIGNAL(notifyNewData(QByteArray)),
            this, SLOT(catch_asynchronous_update(QByteArray)));
}

QString DA_ConfigFile::fileName() const
{
    return get_file_transport()->fileName();
}

void DA_ConfigFile::reload()
{
    get_file_transport()->reload();
}

void DA_ConfigFile::getIdentity(QString &identifier, QString &modifier)
{
    identifier = _identity;
    modifier = _modifier;
}

void DA_ConfigFile::value_changed()
{
    // Export the changed data to the config file
    throw GUtil::NotImplementedException();
    //enQueue(true);
    //exportData();
}

Private::FileTransport *DA_ConfigFile::get_file_transport() const
{
    return (Private::FileTransport *)_transport;
}

QString DA_ConfigFile::get_file_location(QString id)
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

void DA_ConfigFile::catch_asynchronous_update(const QByteArray &dat)
{
    importData(dat);

    emit notifyConfigurationUpdate();
}
