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

#include "giodevicebundlemanager.h"
#include "DataAccess/gfileiodevice.h"
#include "DataObjects/DataSet/datatable.h"
#include <QObject>
#include <QString>

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

        class ConfigFile :
                public GIODeviceBundleManager,
                public GIODeviceBundleManager::DerivedClassFunctions,
                public Core::Interfaces::IUpdatable
        {
            Q_OBJECT
        public:

            explicit ConfigFile(const QString &, const QString &modifier = "",
                                QObject *parent = 0);
            explicit ConfigFile(const ConfigFile &, QObject *parent = 0);
            ~ConfigFile();


            Custom::GVariant Value(const QString &key);
            QMap<QString, Custom::GVariant> Values(
                    const QStringList &keys = QStringList());

            void SetValue(const QString &key, const Custom::GVariant& value);
            void SetValues(const QMap<QString, Custom::GVariant> &);

            // Remove a specific key (or keys)
            void RemoveValue(const QString &);
            void RemoveValues(const QStringList &);

            bool Contains(const QString &key);

            void Clear();


            bool Reload();

            inline QString FileName() const{ return FileTransport().FileName(); }

            inline void GetIdentity(QString &identifier, QString &modifier) const{
                identifier = _identity;
                modifier = _modifier;
            }

            // If the configuration is supposed to be interfaced with humans,
            //  then set this to true.  It not, then the configuration will be
            //  in a compressed file that only the application can read.
            // Note: You must define "CRYPTOPP_COMPRESSION" in the preprocessor
            //  and link with libcryptopp.a to benefit from compression.
            PROPERTY(IsHumanReadable, bool);

            PROPERTY(AutoCommitChanges, bool);


        signals:

            void NotifyConfigurationUpdate();


        protected:

            inline DataAccess::GFileIODevice &FileTransport() {
                return (DataAccess::GFileIODevice &)transport();
            }
            inline const DataAccess::GFileIODevice &FileTransport() const {
                return (const DataAccess::GFileIODevice &)transport();
            }

            // IUpdatable interface:
            void commit_reject_changes(bool commit);


        protected slots:

            void new_input_data_arrived();


        private:

            QString _identity;
            QString _modifier;

            DataObjects::DataTable _table;

            QWaitCondition _condition_config_update;

            static QString get_file_location(QString id);
            void _init(const QString &, const QString &);

            // Overrides of GIODeviceBundleManager functions
            //  (to support compression of the data)
            void preprocess_outgoing_data(QByteArray &) const;
            void preprocess_incoming_data(QByteArray &) const;

            void init_new_table(DataObjects::DataTable &) const;

            void _value_changed();

        };
    }
}

#endif // DA_CONFIGFILE_H
