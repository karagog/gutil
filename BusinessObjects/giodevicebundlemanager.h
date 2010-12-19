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
#include "DataAccess/giodevice.h"
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

    namespace BusinessObjects
    {
        class IODevicePackage;



        // Serves as a generic class to hold values and
        //  send/receive them over the provided transport mechanisms.

        // Derived classes must implement the NewDataProcessor class and
        //  inject a pointer to themselves into the constructor

        // Do NOT use from more than 1 thread

        class GIODeviceBundleManager :
                public QObject
        {
            Q_OBJECT
        signals:

            void NewDataArrived(const QUuid &id = QUuid());


        public:

            // Returns a GUID that each message is stamped with
            QUuid SendData(const DataObjects::DataTable &,
                           const QUuid &id = QUuid());

            // Use this to safely remove an item from the in_queue
            DataObjects::DataTable ReceiveData(const QUuid &id = QUuid());
            bool HasData(const QUuid &id = QUuid());

            inline bool GetAsyncWrite(const QUuid &id = QUuid()) const{
                return _get_package(id)->GetAsyncWrite();
            }
            inline void SetAsyncWrite(bool b, const QUuid &id = QUuid ()) const{
                _get_package(id)->SetAsyncWrite(b);
            }


            // Insert an IO device into the bundle
            // This object will take ownership of the io device and delete it
            //   when it's finished with it
            void InsertIntoBundle(DataAccess::GIODevice *iodevice);
            inline void AddToBundle(DataAccess::GIODevice *io){
                InsertIntoBundle(io);
            }

            void Remove(const QUuid &);
            inline void Remove(DataAccess::GIODevice *io){
                Remove(io->GetIdentity());
            }


            // Derived classes use this to control the behavior of the AVB
            class DerivedClassFunctions
            {
                friend class GIODeviceBundleManager;
            protected:

                // Use these functions to do anything to the data before it
                //  goes and just before it comes in.  For example you might
                //  do things like encrypt/compress the data.
                virtual void preprocess_incoming_data(QByteArray &) const{}
                virtual void preprocess_outgoing_data(QByteArray &) const{}

            };

        protected:

            // No public constructor; this class must be derived.
            GIODeviceBundleManager(DerivedClassFunctions *dp = 0,
                                   QObject *parent = 0);

            ~GIODeviceBundleManager();

            // Derived classes MUST call this on the first line of their
            //  destructor
            void remove_all_iodevices();

            // The method of transport (could be file, socket, network I/O)
            inline DataAccess::GIODevice &transport(const QUuid &id = QUuid()){
                return *_get_package(id)->IODevice;
            }
            inline const DataAccess::GIODevice &transport(
                    const QUuid &id = QUuid()) const{
                return *_get_package(id)->IODevice;
            }

            // Blocks until the message denoted by the unique identifier is sent
            void wait_for_message_sent(const QUuid &message_id,
                                       const QUuid &iodevice_id = QUuid());


            enum QueueTypeEnum
            {
                InQueue,
                OutQueue
            };


        protected slots:

            // This is called automatically when the transport layer
            //   says there's new data
            void importData(const QUuid &id = QUuid());


        private:

            // This represents one IO Device in the bundle bundle
            class IODevicePackage
            {
            public:

                inline IODevicePackage(DataAccess::GIODevice *dev)
                    :IODevice(dev),
                    FlagNewIncomingDataReady(false),
                    FlagNewOutgoingDataEnqueued(false),
                    FlagCancel(false)
                {}

                ~IODevicePackage(){
                    delete IODevice;
                }

                DataAccess::GIODevice *IODevice;

                PROPERTY( AsyncWrite, bool );

                QMutex InQueueMutex;
                QMutex OutQueueMutex;
                QQueue<DataObjects::DataTable> InQueue;
                QQueue<DataObjects::DataTable> OutQueue;

                // So people can block until their data is written
                QWaitCondition ForDataWritten;

                // A reference to the worker thread
                QFuture<void> Worker;

                // These booleans command the worker thread, and are protected
                //  by the flags mutex.  Wake the thread with the wait condition,
                //  after setting the booleans for what it's supposed to do
                bool FlagNewIncomingDataReady;
                bool FlagNewOutgoingDataEnqueued;
                bool FlagCancel;

                QMutex FlagsMutex;
                QWaitCondition ForSomethingToDo;

            };

            QMap<QUuid, IODevicePackage *> _iodevices;


            void _flush_out_queue(IODevicePackage *);
            void _receive_incoming_data(IODevicePackage *);

            // The body for the background worker thread
            void _worker_thread(IODevicePackage *io_package);

            DerivedClassFunctions *_derived_class_pointer;

            IODevicePackage *_get_package(const QUuid &) const;

        };
    }
}

#endif // DO_VALUEBUFFER_H
