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
        // Serves as a generic class to hold values and
        //  send/receive them with the provided transport mechanism.

        // Derived classes must implement the NewDataProcessor class and
        //  inject a pointer to themselves into the constructor

        // Do NOT use from more than 1 thread

        class AbstractValueBuffer :
                public QObject
        {
            Q_OBJECT
        public:

            // Should we send data asynchronously, or block until we've sent it?
            PROPERTY( AsyncWrite, bool );


            // Derived classes must inherit this also, because the background
            //  threads don't always work with virtual functions inside AVB:
            class DerivedClass
            {
                friend class AbstractValueBuffer;
            protected:

                // Use these functions to do anything to the data before it
                //  goes and just before it comes in.  For example you might
                //  do things like encrypt/compress the data.
                virtual void preprocess_incoming_data(QByteArray &) const{}
                virtual void preprocess_outgoing_data(QByteArray &) const{}

                virtual void new_input_data_arrived(
                        const DataObjects::DataTable &){}

            };


        protected:

            // No public constructor; this class must be derived.
            AbstractValueBuffer(
                    DataAccess::GIODevice *transport,
                    DerivedClass *dp = 0,
                    QObject *parent = 0);

            ~AbstractValueBuffer();

            // Derived classes must call this to start the worker threads.
            //  This is because it depends on a pure virtual method, so this
            //  class' vtable must be set up when the threads run
            void start_worker_threads();

            // The method of transport (could be file, socket, network I/O)
            inline DataAccess::GIODevice &transport(){
                return *_transport;
            }
            inline const DataAccess::GIODevice &transport() const{
                return *_transport;
            }

            // Access the table of data
            inline DataObjects::DataTable &table(){
                return _cur_data;
            }
            inline const DataObjects::DataTable &table() const{
                return _cur_data;
            }

            // Make sure to lock this table, if it might be updated in a
            //  background thread
            inline QMutex &table_lock(){
                return _cur_data_lock;
            }

            // Forcefully remove all data from the queue
            void clearQueues();

            // Blocks until the message denoted by the unique identifier is sent
            void wait_for_message_sent(const QUuid &);


            enum QueueTypeEnum
            {
                InQueue,
                OutQueue
            };

            // Use this to safely remove an item from the in_queue
            DataObjects::DataTable deQueueMessage(QueueTypeEnum);

            // Use this to enqueue the current message for sending
            QUuid enQueueCurrentData(bool clear = true);


        protected slots:

            // This is called automatically when the transport layer
            //   says there's new data
            void importData();


        private:

            // Subclasses inject this dependency, which in most cases is
            //  a pointer to themselves
            DerivedClass *_derived_class_pointer;


            DataObjects::DataTable en_deQueueMessage(
                    QueueTypeEnum,
                    const DataObjects::DataTable &msg,
                    bool enqueue);

            void _get_queue_and_mutex(QueueTypeEnum,
                                      QQueue<DataObjects::DataTable> **,
                                      QMutex **);

            QMutex _in_queue_mutex;
            QMutex _out_queue_mutex;
            QWaitCondition _condition_outgoing_data_sent;

            QMutex _incoming_flags_mutex;
            QMutex _outgoing_flags_mutex;
            QWaitCondition _condition_incoming_data_enqueued;
            QWaitCondition _condition_outgoing_data_enqueued;

            QQueue<DataObjects::DataTable> _in_queue;
            QQueue<DataObjects::DataTable> _out_queue;

            DataObjects::DataTable _cur_data;
            QMutex _cur_data_lock;

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

            void _clear_queue(QMutex &, QQueue< DataObjects::DataTable > &);

            DataAccess::GIODevice *_transport;

        };
    }
}

#endif // DO_VALUEBUFFER_H
