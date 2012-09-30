/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include "persistentdata.h"
#include "gutil_varianttable.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileSystemWatcher>
#include <QDesktopServices>
#include <QDir>
#include <QTimer>
USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL1(Interfaces);
USING_NAMESPACE_GUTIL2(QT, DataObjects);
USING_NAMESPACE_GUTIL2(QT, DataAccess);

NAMESPACE_GUTIL2(QT, BusinessObjects);


PersistentData::PersistentData(const String &identifier,
                       const String &modifier,
                       QObject *parent)
    :QObject(parent),
      _p_IsHumanReadable(true),
      _p_AutoCommitChanges(true),
      _identity(identifier),
      _modifier(modifier)
{}

PersistentData::PersistentData(const PersistentData &other)
    :QObject(other.parent()),
      IUpdatable(),
      _p_IsHumanReadable(other._p_IsHumanReadable),
      _p_AutoCommitChanges(other._p_AutoCommitChanges),
      _identity(other._identity),
      _modifier(other._modifier)
{}

void PersistentData::_init(const String &identity, const String &modifier)
{
    _iodevice = new FileIODevice(QString("%1%2%3.config.xml")
                                  .arg(get_file_location(identity))
                                  .arg(modifier.IsEmpty() ? "" : ".")
                                  .arg(modifier.IsEmpty() ? "" : modifier.ConstData()));
    connect(_iodevice, SIGNAL(ReadyRead()),
            this, SLOT(new_input_data_arrived()));

    // Set the file transport to overwrite the config file rather than append
    FileTransport().SetWriteMode(FileIODevice::WriteOver);

    if(!Reload());
}

bool PersistentData::Reload()
{
    FailIfNotInitialized();
    bool ret(true);
    VariantTable tbl;
    try
    {
        QByteArray d( FileTransport().FileData() );
        _preprocess_incoming_data(d);

        tbl.FromXmlQString(d);
    }
    catch(...)
    {
        ret = false;
    }

    if(ret && tbl.ColumnCount() != 2)
        ret = false;

    if(ret)
    {
        m_data.Clear();
        for(VariantTable::ConstRowIterator iter(tbl.RowsBegin()); iter != tbl.RowsEnd(); ++iter)
        {
            QByteArray ba(iter[0].toByteArray());
            m_data.Insert(String(ba.constData(), ba.length()), iter[1]);
        }
    }

    return ret;
}



#ifdef GUTIL_CRYPTOPP

#define CRYPTOPP_DECOMPRESS( data ) \
        Utils::CryptoHelpers::decompress( \
                    std::string(data.constData(), data.length()))

#define CRYPTOPP_COMPRESS( data ) \
        Utils::CryptoHelpers::compress( \
                    std::string(data.constData(), data.length()))

#else

#define CRYPTOPP_DECOMPRESS( data ) \
                    std::string(data.constData(), data.length())

#define CRYPTOPP_COMPRESS( data ) \
                    std::string(data.constData(), data.length())

#endif

void PersistentData::_preprocess_outgoing_data(QByteArray &ba) const
{
    if(!GetIsHumanReadable())
    {
        // Compress the configuration data
        std::string tmpres = CRYPTOPP_COMPRESS(ba);
        ba = QByteArray(tmpres.c_str(), tmpres.length());
    }
}

void PersistentData::_preprocess_incoming_data(QByteArray &data) const
{
    // try to decompress it,
    try
    {
        data = CRYPTOPP_DECOMPRESS(data).c_str();
    }
    catch(const Exception<> &)
    {
        // If decompression fails we ignore it 'cause it's a human-readable file
    }
}

void PersistentData::new_input_data_arrived()
{
    if(Reload())
        emit NotifyConfigurationUpdate();
}


QVariant PersistentData::Value(const String &key) const
{
    FailIfNotInitialized();
    QVariant ret;
    if(m_data.Contains(key))
        ret = m_data.At(key);
    return ret;
}

Map<String, QVariant> PersistentData::Values(const StringList &keys) const
{
    FailIfNotInitialized();
    Map<String, QVariant> ret;
    for(StringList::const_iterator iter(keys.begin()); iter != keys.end(); ++iter)
    {
        QVariant tmp;
        if(m_data.Contains(*iter)) tmp = m_data.At(*iter);
        ret.Insert(*iter, tmp);
    }
    return ret;
}

QString PersistentData::get_file_location(QString id)
{
    QString data_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    QDir tmpdir(data_path);
    if(!tmpdir.exists())
    {
        if(!tmpdir.mkpath(data_path))
            return QString::null;
    }

    return QString("%1/%2")
            .arg(tmpdir.absolutePath())
            .arg(id);
}

void PersistentData::commit_reject_changes(bool commit)
{
    if(commit)
    {
        FailIfNotInitialized();

        // Populate a variant table with the settings data
        VariantTable tbl(2);
        for(Map<String, QVariant>::const_iterator iter(m_data.begin()); iter != m_data.end(); ++iter)
            tbl.AddRow(Vector<QVariant>(QVariant(iter->Key().ConstData())) << iter->Value());

        QByteArray data( tbl.ToXmlQString().toAscii() );

        _preprocess_outgoing_data(data);

        // Export the changed data to the config file
        //  (don't clear the current data in it)
        _iodevice->SendData(data);

        // Notify immediately; the signal is suppressed when the file is loaded
        //  and found to be the same data, so it is only emitted once.
        emit NotifyConfigurationUpdate();
    }
}


END_NAMESPACE_GUTIL2;

#endif // GUI_FUNCTIONALITY
