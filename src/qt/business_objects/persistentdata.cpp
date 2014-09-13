/*Copyright 2010-2013 George Karagoulis

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
#ifndef GUTIL_NO_DATABASE_FUNCTIONALITY

#include "persistentdata.h"
#include "gutil_varianttable.h"
#include "gutil_set.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFileSystemWatcher>
#include <QDesktopServices>
#include <QDir>
#include <QTimer>
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(QT);


void PersistentData::_clear_data_index(QMap<QString, data_t *> &data, QMap<int, data_t *> &index)
{
    for(QMap<QString, data_t *>::iterator iter(data.begin()); iter != data.end(); ++iter)
        delete *iter;
    data.clear();
    index.clear();
}


PersistentData::PersistentData()
{
    THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
}

PersistentData::PersistentData(const QString &identifier, const QString &modifier, QObject *parent)
    :QObject(parent),
      m_bds(QString("%1%2%3.GUtilPersistentData.sqlite")
            .arg(_get_file_location(identifier))
            .arg(modifier.isEmpty() ? "" : ".")
            .arg(modifier.isEmpty() ? "" : modifier.toUtf8().constData())),
      _p_AutoCommitChanges(true)
{
    connect(&m_watcher, SIGNAL(fileChanged(const QString &)),
        this, SLOT(Reload()));

    m_identity = identifier;
    m_modifier = modifier;
    Reload();

    m_watcher.addPath(m_bds.GetFileName());
}

PersistentData::~PersistentData()
{
    _clear_data_index(m_data, m_index);
}

void PersistentData::Clear()
{
    _clear_data_index(m_data, m_index);
    _value_changed();
}

void PersistentData::_process_line_of_data(QByteArray ba, QByteArray *key, QVariant *value)
{
    // Do some pre-processing on the raw data
    preprocess_incoming_data(ba);

    int split_index( ba.indexOf(':') );

    if(-1 != split_index){
        *key = ba.left(split_index);
        *value = Variant::ConvertFromXmlQString(ba.right(ba.length() - split_index - 1));
    }
    else{
        GASSERT(false);
    }
}

void PersistentData::Reload()
{
    bool something_changed(false);
    Vector< Pair<int, QUuid> > ids( m_bds.GetIds() );

    Set<int> my_ids;
    for(QMap<int, data_t *>::iterator iter(m_index.begin()); iter != m_index.end(); ++iter)
        my_ids.Insert(iter.key());

    for(Vector< Pair<int,QUuid> >::iterator iter(ids.begin()); iter != ids.end(); ++iter)
    {
        int data_id =iter->First;
        QUuid data_version = iter->Second;

        typename QMap<int, data_t *>::const_iterator tmp_d = m_index.find(iter->First);
        bool found = tmp_d != m_index.end();

        if(!found || (*tmp_d)->Version != data_version)
        {
            something_changed = true;
            QByteArray ba_key;
            QVariant value;
            _process_line_of_data(m_bds.GetData(data_id), &ba_key, &value);

            if(found)
            {
                // Update our memory store with new values
                (*tmp_d)->Data = value;
                (*tmp_d)->Version = data_version;
                (*tmp_d)->Dirty = false;
            }
            else
            {
                // Insert new values in our memory store
                String key(ba_key.constData(), ba_key.length());
                key = key.FromBase64();

                QString q_key = key.ToQString();
                data_t *d = new data_t(q_key, value, data_id, iter->Second);
                m_data.insert(q_key, d);
                m_index.insert(iter->First, d);
            }
        }

        // Keep track of all the ids we come across, so we know if we need to remove some
        if(my_ids.Contains(data_id))
            my_ids.RemoveOne(data_id);
    }

    // Remove the data that doesn't exist in the file
    for(Set<int>::const_iterator iter(my_ids.begin()); iter != my_ids.end(); ++iter)
    {
        GASSERT(m_index.find(*iter) != m_index.end());

        data_t *d = *m_index.find(*iter);

        m_data.remove(d->Key);
        m_index.remove(d->Id);
        delete d;
        something_changed = true;
    }

    if(something_changed)
        emit DataChanged();
}

void PersistentData::preprocess_outgoing_data(QByteArray &)
{ /** Nothing by default */ }

void PersistentData::preprocess_incoming_data(QByteArray &)
{ /** Nothing by default */ }


QVariant PersistentData::Value(const QString &key) const
{
    QVariant ret;
    typename QMap<QString, data_t *>::const_iterator tmp = m_data.find(key);
    if(tmp != m_data.end())
        ret = (*tmp)->Data;
    return ret;
}

void PersistentData::SetValue(const QString &key, const QVariant &value)
{
    typename QMap<QString, data_t *>::const_iterator tmp = m_data.find(key);
    if(tmp != m_data.end()){
        // Need to update existing data
        (*tmp)->Data = value;
        (*tmp)->Version = QUuid::createUuid();
        (*tmp)->Dirty = true;
    }
    else{
        // Need to insert the new data
        m_data.insert(key, new data_t(key, value));
    }
    _value_changed();
}

void PersistentData::RemoveValue(const QString &key)
{
    typename QMap<QString, data_t *>::const_iterator d = m_data.find(key);
    if(d != m_data.end()){
        const int id = (*d)->Id;

        delete *d;
        m_data.remove(key);
        m_index.remove(id);

        _value_changed();
    }
}

QStringList PersistentData::Keys() const
{
    return m_data.keys();
}

void PersistentData::_value_changed()
{
    if(GetAutoCommitChanges())
        CommitChanges();
}

QString PersistentData::_get_file_location(QString id)
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
        // Commit changes to the object
        bool something_changed = false;
        Set<int> keep_ids;

        // Add/Update data that is in our memory store
        for(QMap<QString, data_t *>::iterator iter(m_data.begin()); iter != m_data.end(); ++iter)
        {
            data_t *d = *iter;
            if(d->Dirty)
            {
                QByteArray new_data(QString("%1:%2")
                                    .arg(String::FromQString(iter.key()).ToBase64().ToQString())
                                    .arg(Variant::ConvertToXmlQString(d->Data, false)).toUtf8());

                preprocess_outgoing_data(new_data);

                if(-1 == d->Id){
                    Pair<int, QUuid> p(m_bds.AddData(new_data));
                    d->Id = p.First;
                    d->Version = p.Second;
                    m_index.insert(d->Id, d);
                }
                else{
                    d->Version = m_bds.SetData(d->Id, new_data);
                }
                d->Dirty = false;
                something_changed = true;
            }

            keep_ids.Insert(d->Id);

            GASSERT(d->Id != -1);   // All data must have an id here
        }

        // Remove data from disk that is not in our memory store
        Vector<Pair<int, QUuid> > disk_ids = m_bds.GetIds();
        for(GUINT32 i = 0; i < disk_ids.Length(); ++i){
            int cur_id = disk_ids[i].First;
            if(!keep_ids.Contains(cur_id)){
                m_bds.RemoveData(cur_id);
                something_changed = true;
            }
        }

        if(something_changed)
            emit DataChanged();
    }
    else
    {
        // Reject changes to the object
        Reload();
    }
}


END_NAMESPACE_GUTIL1;

#endif // GUTIL_NO_DATABASE_FUNCTIONALITY
#endif // GUI_FUNCTIONALITY
