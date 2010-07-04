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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QMap>
#include <QString>

namespace GUtil
{
    namespace QtUtil
    {
        class UserMachineLock;

        // This encapsulates persistent settings for your application

        // This class is NOT thread-safe, so don't share the same Settings object
        //  between several threads.  However, you CAN use several 'Settings'
        //  objects and access the same settings identifier from different applications
        //  at the same time.  It implements a file-locking mechanism to make sure
        //  that the writer has exclusive access, and the others block until the
        //  the lock is released, but they never hold the lock for long so the blocking
        //  time is minimal.  If you want to use the same settings identifier from different
        //  threads of the same application, then you have to implement your own locking.

        class Settings : public QObject
        {
            Q_OBJECT
        public:

            // The identity with which you initialize this settings object
            //  will determine the name of the settings file that it looks
            //  at.  You don't care what the filename is because that's all
            //  managed for you, but you can create a separate settings
            //  object and access the same values as any other settings
            //  object with the same identity.  (careful when modifying the
            //  settings though, because all other settings objects with
            //  the same identity will not be notified of the changes!  To
            //  force an update manually, call 'Reload()')
            Settings(const QString &identity);
            ~Settings();

            // Re-read the settings file
            bool Reload();

            bool SetValue(const QString &key, const QString& value);
            bool SetValues(const QMap<QString, QString> &);

            QString Value(const QString &key);
            const QMap<QString, QString> Values(const QStringList &);
            const QString operator [](const QString &) const;

            bool IsLoaded() const;
            bool Contains(const QString &key);

            // Erase all settings
            bool Clear();

            // Remove a specific key
            bool Remove(const QString &);
            bool Remove(const QStringList &);

            QString Identity() const;
            QString FileName() const;

            QString Error() const;

        signals:
            void NotifySaved();

        private slots:
            void clear_error();
            bool save_settings();
            bool load_settings(const QString &);

        private:
            QString _error;
            QMap<QString, QString> _values;
            QString _identity_string;
            QString _settings_filename;

            UserMachineLock *_userlock;
            bool _lock_acquired;

            bool _loaded;

            QString get_settings_location();

            void set_error(const QString &err = "");
        };

    }
}

#endif // SETTINGS_H
