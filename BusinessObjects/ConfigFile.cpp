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
    :QObject(parent),
    _p_IsHumanReadable(true),
    _p_AutoCommitChanges(true),
    _device_manager(this)
{
    connect(&_device_manager, SIGNAL(NewDataArrived()),
            this, SLOT(new_input_data_arrived()));

    _device_manager.InsertIntoBundle(
            new DataAccess::GFileIODevice(QString("%1.%2")
                                          .arg(get_file_location(identifier))
                                          .arg(modifier)));

    // Set the file transport to overwrite the config file rather than append
    FileTransport().SetWriteMode(DataAccess::GFileIODevice::WriteOver);

    _init(identifier, modifier);
}

ConfigFile::ConfigFile(const BusinessObjects::ConfigFile &other,
                       QObject *parent)
    :QObject(parent),
    Core::Interfaces::IUpdatable(),
    _p_IsHumanReadable(other._p_IsHumanReadable),
    _p_AutoCommitChanges(other._p_AutoCommitChanges),
    _device_manager(this)
{
    _device_manager.InsertIntoBundle(
            new DataAccess::GFileIODevice(other.FileName()));

    _init(other._identity, other._modifier);
}

void ConfigFile::_init(const QString &identity, const QString &modifier)
{
    _identity = identity;
    _modifier = modifier;

    if(!Reload())
        _init_new_table(_table);
}

bool ConfigFile::Reload()
{
    bool ret(true);
    QByteArray d;

    try
    {
        d = FileTransport().FileData();
        _preprocess_incoming_data(d);
    }
    catch(Core::Exception &)
    {
        ret = false;
    }

    if(ret)
    {
        _table.LockForWrite();
        try
        {
            _table.FromXmlQString(d);
        }
        catch(Core::Exception &)
        {
            ret = false;
        }
        _table.Unlock();
    }
    return ret;
}

void ConfigFile::Clear()
{
    _table.LockForWrite();
    {
        _table.Clear();
        _init_new_table(_table);

        _value_changed();
    }
    _table.Unlock();
}

void ConfigFile::_init_new_table(DataObjects::DataTable &tbl) const
{
    // Two columns to the table
    tbl.SetColumnHeaders(QStringList("key") << "value");
}

void ConfigFile::_value_changed()
{
    if(GetAutoCommitChanges())
        CommitChanges();
}




#define CRYPTOPP_DECOMPRESS( data ) \
        Core::Utils::CryptoHelpers::decompress( \
                    std::string(data.constData(), data.length()))

#define CRYPTOPP_COMPRESS( data ) \
        Core::Utils::CryptoHelpers::compress( \
                    std::string(data.constData(), data.length()))

void ConfigFile::_preprocess_outgoing_data(QByteArray &ba) const
{
    if(!GetIsHumanReadable())
    {
        // Compress the configuration data
        std::string tmpres = CRYPTOPP_COMPRESS(ba);
        ba = QByteArray(tmpres.c_str(), tmpres.length());
    }
}

void ConfigFile::_preprocess_incoming_data(QByteArray &data) const
{
    // try to decompress it,
    try
    {
        data = CRYPTOPP_DECOMPRESS(data).c_str();
    }
    catch(Core::Exception &)
    {
        // If decompression fails we ignore it 'cause it's a human-readable file
    }
}

void ConfigFile::new_input_data_arrived()
{
    try
    {
        DataObjects::DataTable tbl;
        QByteArray data(_device_manager.ReceiveData());

        _preprocess_incoming_data(data);

        tbl.FromXmlQString(data);

        // Only update if the table was updated by someone other than us
        if(tbl.NotEquals(_table))
        {
            _table = tbl;

            emit NotifyConfigurationUpdate();
        }
    }
    catch(Core::Exception &)
    {}
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

    _table.LockForWrite();
    {
        foreach(QString s, values.keys())
        {
            bool ex_hit = false;
            try
            {
                DataObjects::DataRow &r = _table.FindFirstRow(0, s);
                r[1] = values[s];
            }
            catch(Core::NotFoundException &)
            {
                ex_hit = true;
            }

            if(ex_hit)
                _table.AddNewRow(Custom::GVariantList() << s << values[s]);
        }
        _value_changed();
    }
    _table.Unlock();
}

Custom::GVariant ConfigFile::Value(const QString &key) const
{
    return Values(QStringList(key)).value(key);
}

QMap<QString, Custom::GVariant> BusinessObjects::ConfigFile::Values(
        const QStringList &keys) const
{
    QMap<QString, Custom::GVariant> ret;
    QStringList keys_copy(keys);

    _table.LockForRead();
    {
        if(keys.isEmpty())
        {
            for(int i = 0; i < _table.RowCount(); i++)
                keys_copy.append(_table[i]["key"].toString());
        }

        foreach(QString s, keys_copy)
        {
            // Prepare the "search map"
            QMap<int, Custom::GVariant> m;
            m.insert(0, s);

            try
            {
                const DataObjects::DataRow *const r = &_table.FindFirstRow(m);
                ret.insert(r->At(0).toString(), r->At(1));
            }
            catch(Core::NotFoundException &){}    // if key not found, ignore
        }
    }
    _table.Unlock();

    return ret;
}

bool ConfigFile::Contains(const QString &key)
{
    bool ret(true);

    _table.LockForRead();
    try
    {
        QMap<int, Custom::GVariant> m;
        m.insert(0, key);
        _table.FindFirstRow(m);
    }
    catch(Core::NotFoundException &)
    {
        ret = false;
    }
    _table.Unlock();

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

    _table.LockForWrite();
    {
        foreach(QString s, keys)
        {
            try
            {
                _table.RemoveRow(
                            _table.FindFirstRow(0, s)
                            );
            }
            catch(Core::NotFoundException &){}
        }

        _value_changed();
    }
    _table.Unlock();
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
    {
        QByteArray data(
                _table.ToXmlQString().toAscii());

        _preprocess_outgoing_data(data);

        // Export the changed data to the config file
        //  (don't clear the current data in it)
        _device_manager.SendData(data);

        // Notify immediately; the signal is suppressed when the file is loaded
        //  and found to be the same data, so it is only emitted once.
        emit NotifyConfigurationUpdate();
    }
}
