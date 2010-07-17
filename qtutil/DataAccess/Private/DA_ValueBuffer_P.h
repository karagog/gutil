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

#ifndef DO_VALUEBUFFER_H
#define DO_VALUEBUFFER_H

#include "DataObjects/variablecontainer.h"
#include <QMap>
#include <QString>
#include <QObject>
#include <QQueue>
#include <QMutex>

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            namespace Transports
            {
                class ITransportMechanism;
            }

            // Serves as a generic class to hold values and send/receive them with
            //   the provided transport mechanism
            class DA_ValueBuffer_P : public QObject
            {
                Q_OBJECT
            public:
                DA_ValueBuffer_P(Transports::ITransportMechanism *, QObject *parent = 0);

                void makeReadOnly(bool val = true);
                bool isReadOnly();

                void setValue(const QString &key, const QString& value);
                void setValues(const QMap<QString, QString> &);

                QString value(const QString &key) const;
                QMap<QString, QString> values(const QStringList &) const;

                bool contains(const QString &key) const;

                void clear();

                // Remove a specific key
                void remove(const QString &);
                void remove(const QStringList &);

            public slots:
                void reload();

            protected:
                // You can achieve different data preparation behavior by overriding these
                virtual QList<QByteArray> prepare_data_for_export();
                virtual void import_data(const QList<QByteArray> &);

                // For manipulating the queues of values
                void enQueue();
                DataObjects::VariableContainer *deQueue();

                // Forcefully remove all data from the queue
                void clearQueue();

                // The method of transport (could be file, socket, network I/O)
                Transports::ITransportMechanism *_transport;

                // These functions actually consume the transport mechanism
                virtual void export_data();
                virtual void reload_data();

            private:
                bool _readonly;

                // This lock protects the _values queue, because it may be enqueued
                //   in a separate thread
                QMutex queue_mutex;

                // We store a queue, becaue we may be buffering several data "blocks"
                QQueue<DataObjects::VariableContainer *> _values;
            };
        }
    }
}

#endif // DO_VALUEBUFFER_H
