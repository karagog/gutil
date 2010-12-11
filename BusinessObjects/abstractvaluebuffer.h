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

#include "DataObjects/DataSet/datatable.h"
#include "Custom/gvariant.h"
#include "Logging/globallogger.h"
#include "Interfaces/iqxmlserializable.h"
#include <QMap>
#include <QString>
#include <QObject>
#include <QQueue>
#include <QReadWriteLock>
#include <QMutex>
#include <QWaitCondition>
#include <QFuture>

namespace GUtil
{
    namespace Core
    {
        class Exception;
    }

    namespace DataObjects
    {
        class DataTable;
    }

    namespace DataAccess
    {
        class GIODevice;
    }

    namespace BusinessObjects
    {
        // Serves as a generic class to hold values and send/receive them with
        //   the provided transport mechanism.

        // Do NOT use from more than 1 thread

        class AbstractValueBuffer : public QObject,
                                    public GUtil::Interfaces::IQXmlSerializable
        {
            Q_OBJECT
        protected:

            // No public constructor; this class must be derived
            AbstractValueBuffer(DataAccess::GIODevice *transport,
                                QObject *parent = 0);

            AbstractValueBuffer(DataAccess::GIODevice *transport,
                                const DataObjects::DataTable &dt = DataObjects::DataTable(),
                                QObject *parent = 0);
            ~AbstractValueBuffer();

            // The method of transport (could be file, socket, network I/O)
            inline DataAccess::GIODevice &transport(){ return *_transport; }
            inline const DataAccess::GIODevice &transport() const{ return *_transport; }

            // Access the table of data
            inline DataObjects::DataTable &table(){ return cur_outgoing_data; }
            inline const DataObjects::DataTable &table() const{ return cur_outgoing_data; }

            inline DataObjects::DataTable &table_incoming(){ return cur_incoming_data; }
            inline const DataObjects::DataTable &table_incoming() const{ return cur_incoming_data; }

            // If you need to do some special data processing, reimplement these
            virtual QByteArray get_current_data(
                    bool human_readable_xml = false) const;
            virtual QString import_incoming_data()
                    throw(Core::Exception);

            // Forcefully remove all data from the queue
            void clearQueues();


            enum QueueTypeEnum
            {
                InQueue,
                OutQueue
            };

            // Use this to safely remove an item from the in_queue
            QByteArray deQueueMessage(QueueTypeEnum);

            // Use this to add a byte array to the current queue
            QUuid enQueueMessage(QueueTypeEnum, const QByteArray &);

            // Use this to prepare to enqueue the current message for sending
            QUuid enQueueCurrentData(bool clear = true);

            // Subclasses implement this to process new data after it comes
            //  in from the GIODevice
            // The default implementation automatically loads the xml
            //  into the "incoming" data table, call it first in an overrided
            //  method, then operate on the incoming DataTable
            virtual void process_input_data(
                    const QPair<QUuid, QByteArray> &);

            virtual void WriteXml(QXmlStreamWriter &) const;
            virtual void ReadXml(QXmlStreamReader &)
                    throw(GUtil::Core::XmlException);


        protected slots:

            // This is called automatically when the transport layer says there's new data
            void importData();


        private:

            QPair<QUuid, QByteArray> en_deQueueMessage(QueueTypeEnum,
                                                       const QByteArray &msg,
                                                       bool enqueue);

            void _get_queue_and_mutex(QueueTypeEnum,
                                      QQueue< QPair<QUuid, QByteArray> > **,
                                      QMutex **);

            QMutex _in_queue_mutex;
            QMutex _out_queue_mutex;

            QMutex _incoming_flags_mutex;
            QMutex _outgoing_flags_mutex;
            QWaitCondition _condition_data_arrived;
            QWaitCondition _condition_data_sent;

            QQueue< QPair<QUuid, QByteArray> > in_queue;
            QQueue< QPair<QUuid, QByteArray> > out_queue;

            DataObjects::DataTable cur_outgoing_data;
            DataObjects::DataTable cur_incoming_data;

            void _init_abstract_value_buffer();

            //void _process_queues();
            void _flush_queue(QueueTypeEnum);

            // The body for the queue managers, which run on
            //  separate threads
            void _queue_processor_thread(
                    QMutex *flags_mutex,
                    QWaitCondition *condition_data_ready,
                    bool *flag_data_ready,
                    int queue_type);

            QFuture<void> _ref_worker_outgoing;
            QFuture<void> _ref_worker_incoming;

            bool _flag_new_outgoing_data_enqueued;
            bool _flag_new_incoming_data_enqueued;

            bool _flag_exiting;

            void _clear_queue(QMutex &, QQueue< QPair<QUuid, QByteArray> > &);

            DataAccess::GIODevice *_transport;

        };
    }
}

#endif // DO_VALUEBUFFER_H
