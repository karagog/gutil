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
#include <QMap>
#include "Private/ValueBuffer.h"

class QtLockedFile;

namespace GUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            class FileTransport;
        }

        // This class provices a persistent storage of variables.  You can store
        //   binary data as well as normal string data, because it translates strings
        //   into base64 before writing them to disk.

        class DA_ConfigFile : public Private::ValueBuffer
        {
            Q_OBJECT
        public:
            explicit DA_ConfigFile(const QString &,
                                   const QString &modifier = "",
                                   QObject *parent = 0);
            explicit DA_ConfigFile(const DA_ConfigFile &, QObject *parent = 0);

            QString fileName() const;

            void getIdentity(QString &identifier, QString &modifier);

        public slots:
            void reload();

        signals:
            void notifyConfigurationUpdate();

        protected:
            virtual void value_changed();

            Private::FileTransport *get_file_transport() const;

        private slots:
            void catch_asynchronous_update(const QByteArray &);

        private:
            bool _ignore_update;

            QString _identity;
            QString _modifier;

            static QString get_file_location(QString id);
            void _init(const QString &, const QString &);
        };
    }
}

#endif // DA_CONFIGFILE_H
