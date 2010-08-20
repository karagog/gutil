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

#include "DataObjects/datacontainer.h"
#include <QMap>
#include <QString>
#include <QObject>
#include <QQueue>
#include <QMutex>

namespace GQtUtil
{
    namespace Interfaces
    {
        class ITransportMechanism;
    }

    namespace DataAccess
    {
        namespace Private
        {
            // Serves as a generic class to hold values and send/receive them with
            //   the provided transport mechanism
            class ValueBuffer : public QObject
            {
                Q_OBJECT
            public:
                ValueBuffer(Interfaces::ITransportMechanism *, QObject *parent = 0);

                void makeReadOnly(bool val = true);
                bool isReadOnly();

                void setValue(const QString &key, const QByteArray& value);
                void setValues(const QMap<QString, QByteArray> &);

                QByteArray value(const QString &key);
                QMap<QString, QByteArray> values(const QStringList &);

                bool contains(const QString &key);

                // Flushes the data queue and clears the current data container
                void clear();

                // Remove a specific key (or keys)
                void remove(const QString &);
                void remove(const QStringList &);


            signals:
                void newDataArrived();


            protected:

                // For manipulating the queues of values
                void enQueue();
                DataObjects::DataContainer *deQueue();

                // Export data through the transport mechanism.  (All but the current data container get exported)
                void export_data();

                // Forcefully remove all data from the queue
                void clearQueue();


            private slots:

                // You never call this directly; It is called when the transport layer says there's new data
                void import_data(const QByteArray &);


            private:
                bool _readonly;

                // This lock protects the _values queue, because it may be enqueued
                //   in a separate thread
                QMutex queue_mutex;

                // We store a queue, becaue we may be buffering several data "blocks"
                QQueue<DataObjects::DataContainer *> _values;

                // The method of transport (could be file, socket, network I/O)
                Interfaces::ITransportMechanism *_transport;

                QList<QByteArray> prepare_data_for_export();
                DataObjects::DataContainer *currentDataContainer();

            };
        }
    }
}

#endif // DO_VALUEBUFFER_H
