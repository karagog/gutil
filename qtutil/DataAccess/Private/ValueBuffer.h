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
#include <QReadWriteLock>

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

                QByteArray& operator [](QString key);

                bool contains(const QString &key);

                // Flushes the data queue and clears the current data container
                void clear();

                // Remove a specific key (or keys)
                void remove(const QString &);
                void remove(const QStringList &);


            protected:

                // This function is called whenever a value changes; derived classes
                //   can take advantage of this to export data or do whatever
                virtual void value_changed();

                // For manipulating the queues of values
                void enQueue(bool copy = false);
                void deQueue();

                // Export data through the transport mechanism.
                //   (All but the current data container get exported)
                void exportData();

                // Forcefully remove all data from the queue
                void clearQueue();

                // The method of transport (could be file, socket, network I/O)
                Interfaces::ITransportMechanism *_transport;


            protected slots:
                // This is called automatically when the transport layer says there's new data
                void importData(const QByteArray &);


            private:
                bool _readonly;

                // This lock protects the _values queue, because it may be enqueued
                //   in a separate thread
                QReadWriteLock queue_lock;
                QReadWriteLock data_container_lock;

                DataObjects::DataContainer *firstContainerInLine();
                DataObjects::DataContainer *currentDataContainer();

                // We store a queue, becaue we may be buffering several data "blocks"
                QQueue<DataObjects::DataContainer *> _values;

                QList<QByteArray> prepare_data_for_export();

                void _enQueue(bool copy = false);
                void _deQueue();

            };
        }
    }
}

#endif // DO_VALUEBUFFER_H
