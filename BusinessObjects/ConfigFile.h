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

#ifndef DA_CONFIGFILE_H
#define DA_CONFIGFILE_H

#include <QObject>
#include <QString>
#include "abstractvaluebuffer.h"

class QtLockedFile;

namespace GUtil
{
    namespace Utils
    {
        class AbstractLogger;
    }

    namespace DataAccess
    {
        class GFileIODevice;
    }

    namespace BusinessObjects
    {
        // This class provices a persistent storage of variables.  You can store
        //   binary data as well as normal string data, because it translates strings
        //   into base64 before writing them to disk.

        class ConfigFile : public AbstractValueBuffer
        {
            Q_OBJECT
        public:
            explicit ConfigFile(const QString &, const QString &modifier = "",
                                QObject *parent = 0);
            explicit ConfigFile(const ConfigFile &, QObject *parent = 0);


            Custom::GVariant Value(const QString &key) const;
            QMap<QString, Custom::GVariant> Values(const QStringList &keys = QStringList()) const;

            void SetValue(const QString &key, const Custom::GVariant& value);
            void SetValues(const QMap<QString, Custom::GVariant> &);

            // Remove a specific key (or keys)
            void RemoveValue(const QString &);
            void RemoveValues(const QStringList &);

            bool Contains(const QString &key) const;


            void Reload();

            QString FileName() const;

            void GetIdentity(QString &identifier, QString &modifier);

            virtual QByteArray get_current_data() const;
            virtual QString import_current_data();

            // If the configuration is supposed to be interfaced with humans,
            //  then set this to true.  It not, then the configuration will be
            //  in a compressed file that only the application can read.
            // Note: You must define "CRYPTOPP_COMPRESSION" in the preprocessor
            //  and link with libcryptopp.a to benefit from compression.
            void SetHumanReadable(bool);
            bool IsHumanReadable() const;

        signals:
            void NotifyConfigurationUpdate();

        protected:

            virtual void process_input_data(const QByteArray &);

            DataAccess::GFileIODevice &FileTransport() const;

            virtual std::string ReadonlyMessageIdentifier() const;

        private:

            QString _identity;
            QString _modifier;

            static QString get_file_location(QString id);
            void _init(const QString &, const QString &);

            void _value_changed();

            bool _config_is_human_readable;
        };
    }
}

#endif // DA_CONFIGFILE_H
