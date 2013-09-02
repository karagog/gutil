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
    inline type Get##name(){ return Value(#name).value<type>(); } \
    inline void Set##name(const type &value){ SetValue( #name, value ); } \
    inline bool Has##name(){ return Contains(#name); } \
    inline void Remove##name(){ RemoveValue(#name); } \
    enum{}



namespace GUtil{ namespace QT{ namespace BusinessObjects{


/** A class to manage the persistent storage of and access to application data.

   This class provices a persistent storage of variables.  You can store
   binary data as well as normal string data, because it translates strings
   into base64 before writing them to disk.
*/
class PersistentData :
        public QObject,
        public GUtil::Interfaces::IUpdatable
{
    Q_OBJECT
    GUTIL_DISABLE_COPY(PersistentData);

    /** Describes a piece of persistent data, along with relevavent metadata. */
    class data_t
    {
    public:
        QVariant Data;
        QUuid Version;
        ::GUtil::DataObjects::String Key;
        int Id;
        bool Dirty;

        inline data_t(const ::GUtil::DataObjects::String &k,
                      const QVariant &d,
                      int id = -1,
                      const QUuid &version = QUuid())
            :Data(d), Version(version), Key(k), Id(id), Dirty(version.isNull() ? true : false) {}
    };

    ::GUtil::DataObjects::String m_identity;
    ::GUtil::DataObjects::String m_modifier;
    ::GUtil::DataObjects::Map< ::GUtil::DataObjects::String, data_t * > m_data;
    ::GUtil::DataObjects::Map<int, data_t *> m_index;

    ::GUtil::QT::BusinessObjects::BinaryDataStore m_bds;
    QFileSystemWatcher m_watcher;

public:

    /** Constructs a persistent data object.
     *
     *  \param identifier A string identifier to distinguish this object from others
     *  \param modifier A string modifier to distinguish this object from others within the application.
    */
    explicit PersistentData(const GUtil::DataObjects::String &identifier,
                            const GUtil::DataObjects::String &modifier = "",
                            QObject *parent = 0);

    /** You should never call this, but it is provided to allow you to create dummy constructors.
     *  It will throw an exception.
    */
    PersistentData();

    virtual ~PersistentData();


    /** Returns a null variant if the key does not exist */
    QVariant Value(const GUtil::DataObjects::String &key) const;

    /** Sets the data at the given key to the given value */
    void SetValue(const GUtil::DataObjects::String &key, const QVariant &value);

    /** Remove a specific key */
    void RemoveValue(const GUtil::DataObjects::String &key);

    /** Returns whether the key is in the config settings. */
    inline bool Contains(const GUtil::DataObjects::String &key){ return m_data.Contains(key); }

    /** Returns the file name of the file which is storing the cached data. */
    inline const QString &GetFileName() const{ return m_bds.GetFileName(); }

    /** Returns the identity string you passed in the constructor. */
    inline GUtil::DataObjects::String const &GetIdentity() const{ return m_identity; }

    /** Returns the modifier string you passed in the constructor. */
    inline GUtil::DataObjects::String const &GetModifier() const{ return m_modifier; }


    /** Controls whether to commit changes right away, or wait until a manual
        call of CommitChanges().
    */
    PROPERTY(AutoCommitChanges, bool);


public slots:

    /** Reloads the config from the file on disk */
    void Reload();

    /** Clears all config parameters. */
    void Clear();


signals:

    /** This signal is emitted whenever the persistent data changes, either spontaneously or
        initiated by this object itself.
    */
    void DataChanged();


protected:

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
    static void _clear_data_index(::GUtil::DataObjects::Map< ::GUtil::DataObjects::String, data_t *> &,
                                  ::GUtil::DataObjects::Map<int, data_t *> &);

    void _value_changed();

};


}}}

#endif // GUTIL_NO_DATABASE_FUNCTIONALITY
#endif // GUTIL_PERSISTENTDATA_H

#endif // GUI_FUNCTIONALITY
