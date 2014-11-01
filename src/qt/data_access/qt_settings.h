/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_QT_SETTINGS_H
#define GUTIL_QT_SETTINGS_H

#include <gutil/settings.h>
#include <gutil/string.h>
#include <QVariant>
#include <QObject>

namespace GUtil{
class Settings;
}

namespace GUtil{ namespace Qt{


/** A wrapper around GUtil::Settings that stores QVariants,
 *  so you can store just about any Qt type.
*/
class Settings :
        public QObject,
        private GUtil::Settings
{
    Q_OBJECT
    QString m_identity;
    QString m_modifier;
public:

    Settings(const char *identity, const char *modifier = "", QObject * = 0);

    /** Returns a null variant if the key does not exist */
    QVariant Value(const GUtil::String &key);

    /** Sets the data at the given key to the given value */
    void SetValue(const GUtil::String &key, const QVariant &value);

    /** Remove a specific key */
    void RemoveValue(const GUtil::String &key);

    /** Returns true if the settings have been modified but not written to disk yet. */
    bool IsDirty();

    /** Writes the changes to disk */
    void CommitChanges();

    /** Returns whether the key is in the config settings. */
    bool Contains(const GUtil::String &key);

    /** Returns the list of keys in the data store. */
    GUtil::StringList Keys();

    /** Returns the file name of the file which is storing the cached data. */
    QString GetFileName() const{ return GUtil::Settings::FileName().ToQString(); }

    /** Returns the identity string you passed in the constructor. */
    QString const &GetIdentity() const{ return m_identity; }

    /** Returns the modifier string you passed in the constructor. */
    QString const &GetModifier() const{ return m_modifier; }


signals:

    void NotifyReloaded();
    void NotifyChangesCommitted();

protected:

    virtual void on_reloaded(){ emit NotifyReloaded(); }
    virtual void on_changes_written(){ emit NotifyChangesCommitted(); }

};


}}

#endif // GUTIL_QT_SETTINGS_H
