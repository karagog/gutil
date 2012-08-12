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

#ifndef GUTIL_CONFIGFILE_H
#define GUTIL_CONFIGFILE_H

#include "gutil_fileiodevice.h"
#include "gutil_iupdatable.h"
#include "gutil_smartpointer.h"
#include "gutil_map.h"
#include <QObject>
#include <QVariant>


/** Derived classes can make convenient property accessors with this macro */
#define CONFIGFILE_PROPERTY( name, type ) \
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

class ConfigFile :
        public QObject,
        public GUtil::Interfaces::IUpdatable
{
    Q_OBJECT
public:

    explicit ConfigFile(const GUtil::DataObjects::String &, const GUtil::DataObjects::String &modifier = "",
                        QObject *parent = 0);
    explicit ConfigFile(const ConfigFile &);

    /** You must call this before you use the object.  It may throw
        an exception, which is why this is not automatically called
        in the constructor
    */
    inline void Initialize(){ _init(_identity, _modifier); }

    /** Returns whether the settings object was initialized */
    inline bool IsInitialized() const{ return _iodevice; }

    /** A convenience function that throws an exception if we're not initialized */
    inline void FailIfNotInitialized() const{
        if(!IsInitialized()) THROW_NEW_GUTIL_EXCEPTION2(Exception, "You must first call Initialize()");
    }

    /** Returns a null variant if the key does not exist */
    QVariant Value(const GUtil::DataObjects::String &key) const;
    GUtil::DataObjects::Map<GUtil::DataObjects::String, QVariant> Values(
            const GUtil::DataObjects::StringList &keys = GUtil::DataObjects::StringList()) const;

    /** Sets the data at the given key to the given value */
    inline void SetValue(const GUtil::DataObjects::String &key, const QVariant &value){
        FailIfNotInitialized();
        m_data[key] = value;
        _value_changed();
    }
    /** Sets several data key values at once */
    inline void SetValues(const GUtil::DataObjects::Map<GUtil::DataObjects::String, QVariant> &values){
        FailIfNotInitialized();
        for(GUtil::DataObjects::Map<GUtil::DataObjects::String, QVariant>::iterator iter(values.begin()); iter != values.end(); ++iter)
            m_data[iter->Key()] = iter->Value();
        if(values.Size() != 0) _value_changed();
    }

    /** Remove a specific key */
    inline void RemoveValue(const GUtil::DataObjects::String &key){
        FailIfNotInitialized();
        m_data.Remove(key);
        _value_changed();
    }

    /** Remove a list of keys */
    void RemoveValues(const GUtil::DataObjects::StringList &keys){
        FailIfNotInitialized();
        for(GUtil::DataObjects::StringList::const_iterator iter(keys.begin()); iter != keys.end(); ++iter)
            m_data.Remove(*iter);
        if(keys.Count() != 0) _value_changed();
    }

    /** Returns whether the key is in the config settings. */
    inline bool Contains(const GUtil::DataObjects::String &key){
        FailIfNotInitialized();
        return m_data.Contains(key);
    }

    /** Clears all config parameters. */
    inline void Clear(){
        FailIfNotInitialized();
        m_data.Clear();
        _value_changed();
    }

    /** Reloads the config from the file on disk */
    bool Reload();

    inline QString FileName() const{ return FileTransport().FileName(); }

    inline GUtil::DataObjects::String const &GetIdentity() const{ return _identity; }
    inline GUtil::DataObjects::String const &GetModifier() const{ return _modifier; }

    /** Controls whether to format the configxml in a human-readable way.

        If the configuration is supposed to be interfaced with humans,
        then set this to true.  It not, then the configuration will be
        in a compressed and unformatted xml file that only the application can read.

        \note You must define GUTIL_CRYPTOPP in the preprocessor
        and link with libcryptopp.a to benefit from compression.
    */
    PROPERTY(IsHumanReadable, bool);

    /** Controls whether to commit changes right away, or wait until a manual
        call of CommitChanges().
    */
    PROPERTY(AutoCommitChanges, bool);


signals:

    /** Notifies that the configuration has been updated, either by this instance
        directly, or someone else updated the file on disk
    */
    void NotifyConfigurationUpdate();


protected:

    /** Returns our file transport */
    inline DataAccess::FileIODevice &FileTransport(){
        return *_iodevice;
    }
    /** Returns our file transport */
    inline const DataAccess::FileIODevice &FileTransport() const{
        return *_iodevice;
    }

    /** IUpdatable interface */
    virtual void commit_reject_changes(bool commit);

    /** This constructor should never be used, and is only declared to allow
     *  forbidden constructor definitions, such as used in GUTIL_DISABLE_COPY.
    */
    inline ConfigFile(){ THROW_NEW_GUTIL_EXCEPTION(NotImplementedException); }


private slots:

    void new_input_data_arrived();


private:

    GUtil::DataObjects::String _identity, _modifier;
    GUtil::DataObjects::Map<GUtil::DataObjects::String, QVariant> m_data;
    GUtil::Utils::SmartPointer<DataAccess::FileIODevice> _iodevice;

    static QString get_file_location(QString id);

    void _init(const GUtil::DataObjects::String &identifier,
               const GUtil::DataObjects::String &modifier);

    // In case you want to do encryption/compression
    void _preprocess_outgoing_data(QByteArray &) const;
    void _preprocess_incoming_data(QByteArray &) const;

    inline void _value_changed(){
        if(GetAutoCommitChanges())
            CommitChanges();
    }

};


}}}

#endif // GUTIL_CONFIGFILE_H

#endif // GUI_FUNCTIONALITY
