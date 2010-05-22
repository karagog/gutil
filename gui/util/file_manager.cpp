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

#include "file_manager.h"
#include "stringhelpers.h"
#include "encryption.h"
#include "exception.h"
#include <QDesktopServices>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QVariant>
#include <QMutex>
#include <iostream>
using namespace GUtil;

QMutex mut;

File_Manager::File_Manager(const QString &passphrase, bool is_secondary)
{
    max_id = 0;
    _transaction = false;
    _was_reset = !is_secondary;

    file_location = get_new_file_loc();
    encryption_passphrase = passphrase;
}

bool File_Manager::beginTransaction()
{
    // Make sure that only one instance is accessing the file
    mut.lock();

    // Read in the xml from the file
    QFile f(file_location);

    bool res;
    if(_was_reset)
        res = f.open(QFile::ReadWrite | QFile::Truncate);
    else
        res = f.open(QFile::ReadOnly);

    if(!res)
        return false;

    QByteArray dat = f.readAll();
    QString xml = QString::fromStdString(string(dat.constData(), dat.length()));
    f.close();

    // Parse it and populate the map
    if(xml.length())
    {
        // Decrypt it first to turn it back into an xml string
        xml = QString::fromStdString(CryptoHelpers::decryptString(
                xml.toStdString(), encryption_passphrase.toStdString()));

        QXmlStreamReader sr(xml);
        if(sr.readNextStartElement())
        {
            while(sr.readNextStartElement())
            {
                max_id = QVariant(sr.attributes().value("i").toString()).toInt();
                _map.insert(max_id++,
                            QString::fromStdString(
                                    StringHelpers::fromBase64(
                                            sr.attributes().value("d").toString().toStdString())));

                // Need to be explicit and read in the end element, otherwise
                //  it will only parse the first file (bummer...)
                while(sr.readNext() != QXmlStreamReader::EndElement);
            }
        }
    }

    _transaction = true;
    _changed = false;
    return true;
}

unsigned int File_Manager::addFile(const QString &data)
{
    if(!_transaction)
        throw new GUtil::Exception("You can only add files within a transaction");

    // Find a free id
    unsigned int __start = max_id;
    while(_map.contains(max_id))
    {
        max_id++;
        if(max_id == __start)
            throw new GUtil::Exception("There are no free id's left");
    }

    _map.insert(max_id, data);
    _changed = true;

    return max_id++;
}

QString File_Manager::getFile(unsigned int id) const
{
    if(!_transaction)
        throw new GUtil::Exception("You can only get files within a transaction");

    return _map.value(id, "");
}

bool File_Manager::endTransaction()
{
    if(_changed)
    {
        // Write the data to a temporary file
        QString file_xml;
        QXmlStreamWriter sw(&file_xml);
        sw.writeStartElement("f");

        foreach(unsigned int i, _map.keys())
        {
            sw.writeStartElement("f");

            sw.writeAttribute("i", QVariant(i).toString());
            sw.writeAttribute("d", QString::fromStdString(StringHelpers::toBase64(
                    _map[i].toStdString())));

            sw.writeEndElement();
        }

        sw.writeEndElement();

        QFile f(file_location);
        if(!f.open(QFile::Truncate | QFile::WriteOnly))
            return false;

        // Encrypt the files before writing them to disk
        file_xml = QString::fromStdString(CryptoHelpers::encryptString(
                file_xml.toStdString(), encryption_passphrase.toStdString()));

        if(f.write(file_xml.toStdString().c_str(), file_xml.length())
            != file_xml.length())
            return false;
        f.close();
    }

    // Clear the map so we free all the memory used to store these files
    _map.clear();

    _transaction = false;
    _was_reset = false;

    mut.unlock();
    return true;
}

void File_Manager::reset()
{
    if(_transaction)
    {
        _transaction = false;
        _map.clear();
    }

    _was_reset = true;
    max_id = 0;

    file_location = get_new_file_loc();
}

QString File_Manager::passphrase()
{
    return encryption_passphrase;
}

QString File_Manager::get_new_file_loc()
{
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation)
            + "/GPM_temp_data.file";
}
