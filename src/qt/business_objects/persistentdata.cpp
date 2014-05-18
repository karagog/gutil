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


void PersistentData::_clear_data_index(Map<String, data_t *> &data, Map<int, data_t *> &index)
{
    for(Map<String, data_t *>::iterator iter(data.begin()); iter != data.end(); ++iter)
        delete iter->Value();
    data.Clear();
    index.Clear();
}


PersistentData::PersistentData()
{
    THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
}

PersistentData::PersistentData(const String &identifier, const String &modifier, QObject *parent)
    :QObject(parent),
      m_bds(QString("%1%2%3.GUtilPersistentData.sqlite")
            .arg(_get_file_location(identifier))
            .arg(modifier.IsEmpty() ? "" : ".")
            .arg(modifier.IsEmpty() ? "" : modifier.ConstData())),
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
    m_bds.Clear();
    _clear_data_index(m_data, m_index);
}

void PersistentData::Reload()
{
    bool something_changed(false);
    Vector< Pair<int, QUuid> > ids( m_bds.GetIds() );

    Set<int> my_ids;
    for(Map<int, data_t *>::iterator iter(m_index.begin()); iter != m_index.end(); ++iter)
        my_ids.Insert(iter->Key());

    for(Vector< Pair<int,QUuid> >::iterator iter(ids.begin()); iter != ids.end(); ++iter)
    {
        data_t **tmp_d = m_index.Find(iter->First);
        data_t *d = tmp_d ? *tmp_d : 0;
        if(0 == d || d->Version != iter->Second)
        {
            something_changed = true;
            QByteArray ba( m_bds.GetData(iter->First) );

            // Do some pre-processing on the raw data
            preprocess_incoming_data(ba);

            int split_index( ba.indexOf(':') );
            if(-1 == split_index)
                THROW_NEW_GUTIL_EXCEPTION(Exception);

            QVariant v( Variant::ConvertFromXmlQString(ba.right(ba.length() - split_index - 1)) );
            QByteArray ba_key( ba.left(split_index) );

            if(d){
                d->Data = v;
                d->Version = iter->Second;
            }
            else{
                String key(ba_key.constData(), ba_key.length());
                key = key.FromBase64();

                d = new data_t(key, v, iter->First, iter->Second);
                m_data.Insert(key, d);
                m_index.Insert(iter->First, d);
            }
        }

        if(my_ids.Contains(iter->First))
            my_ids.RemoveOne(iter->First);
    }

    // Remove the data that has been removed
    for(Set<int>::const_iterator iter(my_ids.begin()); iter != my_ids.end(); ++iter)
    {
        GASSERT(m_index.Find(*iter));

        data_t *d = *m_index.Find(*iter);

        m_data.Remove(d->Key);
        m_index.Remove(d->Id);
        delete d;
    }


    if(something_changed)
        emit DataChanged();
}

void PersistentData::preprocess_outgoing_data(QByteArray &)
{ /** Nothing by default */ }

void PersistentData::preprocess_incoming_data(QByteArray &)
{ /** Nothing by default */ }


QVariant PersistentData::Value(const String &key) const
{
    QVariant ret;
    data_t* const *tmp = m_data.Find(key);
    if(tmp) ret = (*tmp)->Data;
    return ret;
}

void PersistentData::SetValue(const String &key, const QVariant &value)
{
    data_t **tmp = m_data.Find(key);
    if(tmp){
        // Need to update existing data
        (*tmp)->Data = value;
        (*tmp)->Dirty = true;
    }
    else{
        // Need to insert the new data
        m_data.Insert(key, new data_t(key, value));
    }
    _value_changed();
}

void PersistentData::RemoveValue(const String &key)
{
    data_t **d = m_data.Find(key);
    if(d){
        const int id = (*d)->Id;

        m_bds.RemoveData(id);

        delete *d;
        m_data.Remove(key);
        m_index.Remove(id);

        _value_changed();
    }
}

Vector<String> PersistentData::Keys() const
{
    return m_data.Keys();
}

void PersistentData::_value_changed()
{
    if(GetAutoCommitChanges())
        CommitChanges();

    emit DataChanged();
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
        for(Map<String, data_t *>::iterator iter(m_data.begin()); iter != m_data.end(); ++iter)
        {
            data_t *d = iter->Value();
            if(d->Dirty)
            {
                QByteArray new_data(QString("%1:%2")
                                    .arg(iter->Key().ToBase64().ToQString())
                                    .arg(Variant::ConvertToXmlQString(d->Data, false)).toUtf8());

                preprocess_outgoing_data(new_data);

                if(-1 == d->Id){
                    Pair<int, QUuid> p(m_bds.AddData(new_data));
                    d->Id = p.First;
                    d->Version = p.Second;
                    m_index.Insert(d->Id, d);
                }
                else{
                    d->Version = m_bds.SetData(d->Id, new_data);
                }
                d->Dirty = false;
            }
        }
    }
    else
    {
        // Reject changes to the object
        GASSERT2(false, "ERROR: Rejecting changes not implemented");
    }
}


END_NAMESPACE_GUTIL1;

#endif // GUTIL_NO_DATABASE_FUNCTIONALITY
#endif // GUI_FUNCTIONALITY
