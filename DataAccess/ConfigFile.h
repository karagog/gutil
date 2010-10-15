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
        namespace DataTransports
        {
            class FileTransport;
        }

        // This class provices a persistent storage of variables.  You can store
        //   binary data as well as normal string data, because it translates strings
        //   into base64 before writing them to disk.

        class ConfigFile : public AbstractValueBuffer
        {
            Q_OBJECT
        public:
            explicit ConfigFile(const QString &, const QString &modifier = "",
                                Utils::AbstractLogger *logger = 0,
                                QObject *parent = 0);
            explicit ConfigFile(const ConfigFile &, QObject *parent = 0);
            virtual ~ConfigFile();

            QString fileName() const;

            void getIdentity(QString &identifier, QString &modifier);

        signals:
            void notifyConfigurationUpdate();

        protected:
            virtual void ValueChanged_protected();

            virtual void process_input_data(const QByteArray &);

            DataAccess::DataTransports::FileTransport *FileTransport() const;

            virtual std::string ReadonlyMessageIdentifier() const;

        private slots:
            void catch_asynchronous_update(const QByteArray &);

        private:
            bool _ignore_update;

            QString _identity;
            QString _modifier;

            DataTransports::FileTransport *_file_transport;

            static QString get_file_location(QString id);
            void _init(const QString &, const QString &);
        };
    }
}

#endif // DA_CONFIGFILE_H
