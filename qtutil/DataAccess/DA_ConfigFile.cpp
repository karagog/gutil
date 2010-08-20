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
#include "Private/filetransport.h"
#include "exception.h"
#include "stringhelpers.h"
#include "qtlockedfile.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileSystemWatcher>
#include <QDesktopServices>
#include <QDir>
using namespace GUtil;
using namespace GQtUtil::DataAccess;
using namespace GQtUtil::DataAccess::Private;

DA_ConfigFile::DA_ConfigFile(const QString &identifier, const QString &modifier, QObject *parent)
    :ValueBuffer(new FileTransport(QString("%1.%2")
                                                   .arg(get_file_location(identifier))
                                                   .arg(modifier)),
                      parent)
{
    _ignore_update = false;

    // Attach a filesystem watcher so we can know if the file updates behind our back
    QFileSystemWatcher *fsw = new QFileSystemWatcher(this);
    fsw->addPath(identifier);
    connect(fsw, SIGNAL(fileChanged(QString)), this, SLOT(catch_asynchronous_update()));

    // Automatically load the configuration
    reload();
}

QString DA_ConfigFile::fileName() const
{
    return ((FileTransport *)_transport)->fileName();
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

void DA_ConfigFile::export_data()
{
    // We're the ones updating the file, so we ignore the filesystem watcher
    _ignore_update = true;

    ValueBuffer::export_data();
}

void DA_ConfigFile::catch_asynchronous_update()
{
    if(_ignore_update)
        _ignore_update = false;
    else
    {
        reload();
        emit notifyConfigurationUpdate();
    }
}
