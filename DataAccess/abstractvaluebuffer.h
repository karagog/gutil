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

#include "Core/Interfaces/ireadonlyobject.h"
#include <QMap>
#include <QString>
#include <QObject>
#include <QQueue>
#include <QReadWriteLock>
#include <QMutex>

namespace GUtil
{
    namespace Core
    {
        class Exception;
    }

    namespace DataObjects
    {
        class DataContainer;
    }

    namespace Utils
    {
        class AbstractLogger;
    }

    namespace DataAccess
    {
        namespace DataTransports
        {
            class AbstractDataTransportMechanism;
        }

        // Serves as a generic class to hold values and send/receive them with
        //   the provided transport mechanism
        class AbstractValueBuffer : public QObject, public GUtil::Core::Interfaces::IReadOnlyObject
        {
            Q_OBJECT
        public:
            bool setValue(const QString &key, const QByteArray& value);
            virtual bool setValues(const QMap<QString, QByteArray> &);

            QByteArray value(const QString &key);
            QMap<QString, QByteArray> values(const QStringList &);

            // Remove a specific key (or keys)
            bool removeValue(const QString &);
            bool removeValue(const QStringList &);

            bool contains(const QString &key);

            // Flushes the data queue and clears the current data container
            void clear();


        protected:

            // No public constructor; this class must be derived
            AbstractValueBuffer(DataTransports::AbstractDataTransportMechanism *transport,
                                Utils::AbstractLogger *logger = 0,
                                QObject *parent = 0);
            virtual ~AbstractValueBuffer();

            // The method of transport (could be file, socket, network I/O)
            DataTransports::AbstractDataTransportMechanism &Transport() const;

            // This function is called whenever a value changes; derived classes
            //   can take advantage of this to export data or do whatever with the changed data
            // Throw exceptions when errors happen and they will be logged
            virtual void ValueChanged_protected() = 0;

            // Forcefully remove all data from the queue
            void clearQueues();

            Utils::AbstractLogger *Logger() const;
            void LogException(const GUtil::Core::Exception &) const;


            enum QueueTypeEnum
            {
                InQueue,
                OutQueue
            };

            // Use this to safely remove an item from the in_queue
            QByteArray deQueueMessage(QueueTypeEnum);

            // Use this to add a byte array to the current queue
            void enQueueMessage(QueueTypeEnum, const QByteArray &);

            // Use this to prepare to enqueue the current message for sending
            void enQueueCurrentData(bool clear = true);

            // Subclasses implement this to process a new byte array after it
            //   gets dequeued
            virtual void process_input_data(const QByteArray &) = 0;

            virtual std::string ReadonlyMessageIdentifier() const;


        protected slots:
            // This is called automatically when the transport layer says there's new data
            void importData(const QByteArray &);


        private:

            QByteArray en_deQueueMessage(QueueTypeEnum, const QByteArray &msg, bool enqueue);

            bool ValueChanged();

            QMutex in_queue_mutex;
            QMutex out_queue_mutex;
            QQueue< QByteArray > in_queue;
            QQueue< QByteArray > out_queue;

            QReadWriteLock current_data_lock;
            DataObjects::DataContainer *current_data;

            void process_queues();
            void _flush_queue(QueueTypeEnum);

            void _clear_queue(QMutex &, QQueue< QByteArray > &);

            DataTransports::AbstractDataTransportMechanism *_transport;
            Utils::AbstractLogger *_logger;

        };
    }
}

#endif // DO_VALUEBUFFER_H
