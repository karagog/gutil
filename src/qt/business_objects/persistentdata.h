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

#ifndef GUTIL_PERSISTENTDATA_H
#define GUTIL_PERSISTENTDATA_H

#include "gutil_fileiodevice.h"
#include "gutil_iupdatable.h"
#include "gutil_smartpointer.h"
#include "gutil_map.h"
#include "gutil_binarydatastore.h"
#include <QVariant>
#include <QFileSystemWatcher>


/** Derived classes can make convenient property accessors with this macro */
#define PERSISTENT_PROPERTY( name, type ) \
    type Get##name(){ return Value(#name).value<type>(); } \
    void Set##name(const type &value){ SetValue( #name, value ); } \
    bool Has##name(){ return Contains(#name); } \
    void Remove##name(){ RemoveValue(#name); } \
    enum{}



namespace GUtil{ namespace QT{


/** A class to manage the persistent storage of and access to application data.

   This class provices a persistent storage of variables.  You can store
   binary data as well as normal string data, because it translates strings
   into base64 before writing them to disk.
*/
class PersistentData :
        public QObject,
        public GUtil::IUpdatable
{
    Q_OBJECT
    GUTIL_DISABLE_COPY(PersistentData);

    /** Describes a piece of persistent data, along with relevavent metadata. */
    class data_t
    {
    public:
        QVariant Data;
        QUuid Version;
        QString Key;
        int Id;
        bool Dirty;

        data_t(const QString &k,
                      const QVariant &d,
                      int id = -1,
                      const QUuid &version = QUuid())
            :Data(d), Version(version), Key(k), Id(id), Dirty(version.isNull() ? true : false) {}
    };

    QString m_identity;
    QString m_modifier;
    QMap< QString, data_t * > m_data;
    QMap<int, data_t *> m_index;

    GUtil::QT::BinaryDataStore m_bds;
    QFileSystemWatcher m_watcher;

public:

    /** Constructs a persistent data object.
     *
     *  \param identifier A string identifier to distinguish this object from others
     *  \param modifier A string modifier to distinguish this object from others within the application.
    */
    explicit PersistentData(const QString &identifier,
                            const QString &modifier = QString(),
                            QObject *parent = 0);

    virtual ~PersistentData();


    /** Returns a null variant if the key does not exist */
    QVariant Value(const QString &key) const;

    /** Sets the data at the given key to the given value */
    void SetValue(const QString &key, const QVariant &value);

    /** Remove a specific key */
    void RemoveValue(const QString &key);

    /** Returns whether the key is in the config settings. */
    bool Contains(const QString &key) const{ return m_data.contains(key); }

    /** Returns the list of keys in the data store. */
    QStringList Keys() const;

    /** Returns the file name of the file which is storing the cached data. */
    const QString &GetFileName() const{ return m_bds.GetFileName(); }

    /** Returns the identity string you passed in the constructor. */
    QString const &GetIdentity() const{ return m_identity; }

    /** Returns the modifier string you passed in the constructor. */
    QString const &GetModifier() const{ return m_modifier; }


    /** Controls whether to commit changes right away, or wait until a manual
        call of CommitChanges().
    */
    PROPERTY(AutoCommitChanges, bool);


public slots:

    /** Reloads the config from the file on disk, overwriting any in-memory changes. */
    void Reload();

    /** Clears all config parameters. */
    void Clear();


signals:

    /** This signal is emitted whenever the persistent data changes, either spontaneously or
        initiated by this object itself.
    */
    void DataChanged();


protected:

    /** You should never call this, but it is provided to allow you to create dummy constructors.
     *  It will throw an exception.
    */
    PersistentData();

    /** This function is provided so that subclasses can operate on the data before
     *  it is saved to disk.
     *
     *  One could compress or encrypt the data, if they want.
     *
     *  By default there is no compression or encryption.  You must implement
     *  that in your application code.
    */
    virtual void preprocess_outgoing_data(QByteArray &);

    /** This function is provided so that subclasses can operate on the data after
     *  it is read from disk.
     *
     *  One could decompress or decrypt the data, if they want.
     *
     *  By default there is no compression or encryption.  You must implement
     *  that in your application code.
    */
    virtual void preprocess_incoming_data(QByteArray &);


    /** IUpdatable interface */
    virtual void commit_reject_changes(bool commit);


private:

    static QString _get_file_location(QString id);

    /** Reusable function to clear the indexes and reclaim memory. */
    static void _clear_data_index(QMap<QString, data_t *> &,
                                  QMap<int, data_t *> &);

    void _value_changed();

    void _process_line_of_data(QByteArray ba, QByteArray *key, QVariant *value);

};


}}

#endif // GUTIL_NO_DATABASE_FUNCTIONALITY
#endif // GUTIL_PERSISTENTDATA_H

#endif // GUI_FUNCTIONALITY
