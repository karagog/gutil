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
#include <QThreadPool>

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
        // Serves as a generic class to hold values and
        //  send/receive them over the provided transport mechanisms.

        // It does all of the transport work on background threads, one for
        //  each of the io devices.

        // It is safe to send/receive data on multiple threads; this class
        //  implements the necessary variable locking

        class GIODeviceBundleManager :
                public QObject
        {
            Q_OBJECT
            class IODevicePackage;

        signals:

            void NewDataArrived(const QUuid &id = QUuid(),
                                const QByteArray &data = QByteArray());


        public:

            GIODeviceBundleManager(QObject *parent = 0);
            ~GIODeviceBundleManager();

            // Returns a GUID that each message is stamped with
            QUuid SendData(const QByteArray &,
                           const QUuid &id = QUuid());

            // Use this to safely remove an item from the in_queue
            QByteArray ReceiveData(const QUuid &id = QUuid());
            bool HasData(const QUuid &id = QUuid()) const;

            inline bool GetAsyncWrite(const QUuid &id = QUuid()) const{
                return get_package(id)->GetAsyncWrite();
            }
            inline void SetAsyncWrite(bool b, const QUuid &id = QUuid()) const{
                get_package(id)->SetAsyncWrite(b);
            }

            // Drop all incoming data on the floor
            inline void SetDropInput(bool b, const QUuid &id = QUuid()){
                get_package(id)->SetDropInput(b);
            }
            inline bool GetDropInput(const QUuid &id = QUuid()) const{
                return get_package(id)->GetDropInput();
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

            void RemoveAll(bool wait_until_finished = false);

            // Blocks until the message denoted by the unique identifier is sent
            void WaitForMessageSent(const QUuid &message_id,
                                    const QUuid &iodevice_id = QUuid());

            // The method of transport (could be file, socket, network I/O)
            inline DataAccess::GIODevice &Transport(const QUuid &id = QUuid()){
                return *get_package(id)->IODevice;
            }
            inline const DataAccess::GIODevice &Transport(
                    const QUuid &id = QUuid()) const{
                return *get_package(id)->IODevice;
            }

            inline QList<QUuid> GetIds() const{
                return _iodevices.keys();
            }

            inline int MaxThreads() const{
                return _thread_pool.maxThreadCount();
            }


        protected slots:

            // This is called automatically when the transport layer
            //   says there's new data
            void importData(const QUuid &id = QUuid());


        protected:

            IODevicePackage *get_package(const QUuid &) const;
            QWaitCondition forSomethingToDo;

            class WorkItem
            {
            public:
                enum DirectionEnum{
                    Incoming,
                    Outgoing
                };

                inline WorkItem(const QUuid &id = QUuid(),
                                DirectionEnum direction = Incoming)
                    :Id(id),
                      Direction(direction){}

                QUuid Id;
                DirectionEnum Direction;
            };

            // these objects are protected by the flags mutex
            QQueue<WorkItem> work_queue;
            bool flagCancelThread;
            QMutex flagsMutex;


        private:

            // This represents one IO Device in the bundle
            class IODevicePackage
            {
            public:

                inline IODevicePackage(DataAccess::GIODevice *dev)
                    :IODevice(dev),
                    _p_AsyncWrite(true),
                    _p_DropInput(false)
                {}

                inline ~IODevicePackage(){
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

            };

            class WorkerThread :
                    public QRunnable
            {
            public:
                WorkerThread(GIODeviceBundleManager *bundle_manager)
                    :_bundle_manager(bundle_manager){}

            private:
                void run();
                GIODeviceBundleManager *_bundle_manager;

                void _write_one_packet(IODevicePackage *);
                void _receive_incoming_data(IODevicePackage *);
            };

            QMap<QUuid, IODevicePackage *> _iodevices;

            QThreadPool _thread_pool;
        };
    }
}

#endif // DO_VALUEBUFFER_H
