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

            GIODeviceBundleManager(QObject *parent = 0);
            ~GIODeviceBundleManager();

            // Returns a GUID that each message is stamped with
            QUuid SendData(const QByteArray &,
                           const QUuid &id = QUuid());

            // Use this to safely remove an item from the in_queue
            QByteArray ReceiveData(const QUuid &id = QUuid());
            bool HasData(const QUuid &id = QUuid());

            inline bool GetAsyncWrite(const QUuid &id = QUuid()) const{
                return _get_package(id)->GetAsyncWrite();
            }
            inline void SetAsyncWrite(bool b, const QUuid &id = QUuid()) const{
                _get_package(id)->SetAsyncWrite(b);
            }

            // Drop all incoming data on the floor
            inline void SetDropInput(bool b, const QUuid &id = QUuid()){
                _get_package(id)->SetDropInput(b);
            }
            inline bool GetDropInput(const QUuid &id = QUuid()) const{
                return _get_package(id)->GetDropInput();
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

            void RemoveAll();

            // Blocks until the message denoted by the unique identifier is sent
            void WaitForMessageSent(const QUuid &message_id,
                                    const QUuid &iodevice_id = QUuid());

            // The method of transport (could be file, socket, network I/O)
            inline DataAccess::GIODevice &Transport(const QUuid &id = QUuid()){
                return *_get_package(id)->IODevice;
            }
            inline const DataAccess::GIODevice &Transport(
                    const QUuid &id = QUuid()) const{
                return *_get_package(id)->IODevice;
            }

            inline QList<QUuid> GetIds() const{
                return _iodevices.keys();
            }


        protected slots:

            // This is called automatically when the transport layer
            //   says there's new data
            void importData(const QUuid &id = QUuid());


        private:

            // This represents one IO Device in the bundle
            class IODevicePackage
            {
            public:

                inline IODevicePackage(DataAccess::GIODevice *dev)
                    :IODevice(dev),
                    _p_AsyncWrite(true),
                    _p_DropInput(false),
                    FlagNewIncomingDataReady(false),
                    FlagNewOutgoingDataEnqueued(false),
                    FlagCancel(false)
                {}

                ~IODevicePackage(){
                    // We delete later, because we may get deleted in a function
                    //  triggered by the io device's own signal.
                    IODevice->deleteLater();
                }

                DataAccess::GIODevice *IODevice;

                PROPERTY( AsyncWrite, bool );
                PROPERTY( DropInput, bool );

                QMutex InQueueMutex;
                QMutex OutQueueMutex;
                QQueue<QByteArray> InQueue;
                QQueue< QPair<QUuid, QByteArray> > OutQueue;

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

            IODevicePackage *_get_package(const QUuid &) const;

        };
    }
}

#endif // DO_VALUEBUFFER_H
