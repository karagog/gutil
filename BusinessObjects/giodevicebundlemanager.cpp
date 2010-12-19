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

#include "giodevicebundlemanager.h"
#include "DataAccess/giodevice.h"
#include "Core/Utils/stringhelpers.h"
#include "Core/exception.h"
#include <QStringList>
#include <QtConcurrentRun>
#include <QThreadPool>
using namespace GUtil;
using namespace DataObjects;
using namespace BusinessObjects;

GIODeviceBundleManager::GIODeviceBundleManager(
        DerivedClassFunctions *dp,
        QObject *parent)
            :QObject(parent),
            _derived_class_pointer(dp),
            _current_threads(1)
{}

GIODeviceBundleManager::~GIODeviceBundleManager()
{
    // Derived classes must call this first thing in their destructors, but just
    //  in case we still call it here.  But this may cause a seg fault if called
    //  here
    //kill_worker_threads();

    foreach(QUuid id, _iodevices.keys())
        Remove(id);
}

void GIODeviceBundleManager::kill_worker_threads()
{
    // Loop through and cancel all the threads
    foreach(QUuid id, _iodevices.keys())
    {
        IODevicePackage *pack = _iodevices[id];

        pack->IncomingFlagsMutex.lock();
        pack->OutgoingFlagsMutex.lock();
        {
            pack->FlagCancel = true;
        }
        pack->OutgoingFlagsMutex.unlock();
        pack->IncomingFlagsMutex.unlock();

        pack->ConditionOutgoingDataEnqueued.wakeOne();
        pack->ConditionIncomingDataReadyToRead.wakeOne();
    }

    // Now wait for them to finish
    foreach(QUuid id, _iodevices.keys())
    {
        IODevicePackage *pack = _iodevices[id];

        pack->WorkerOutgoing.waitForFinished();
        pack->WorkerIncoming.waitForFinished();
    }
}

void GIODeviceBundleManager::_get_queue_and_mutex(
        IODevicePackage *pack,
        QueueTypeEnum qt,
        QQueue<DataObjects::DataTable> **q,
        QMutex **m)
{
    switch(qt)
    {
    case InQueue:
        *m = &pack->InQueueMutex;
        *q = &pack->InQueue;
        break;
    case OutQueue:
        *m = &pack->OutQueueMutex;
        *q = &pack->OutQueue;
        break;
    default:
        throw Core::NotImplementedException("Unrecognized queue type");
    }
}

// Wake up our background worker in charge of this ID
void GIODeviceBundleManager::importData(const QUuid &id)
{
    IODevicePackage *pack(_get_package(id));

    pack->IncomingFlagsMutex.lock();
    {
        pack->FlagNewIncomingDataReady = true;
    }
    pack->IncomingFlagsMutex.unlock();

    pack->ConditionIncomingDataReadyToRead.wakeOne();
}

void GIODeviceBundleManager::_flush_out_queue(IODevicePackage *pack)
{
    QQueue<DataTable> *queue;
    QMutex *mutex;

    bool queue_has_data(true);

    _get_queue_and_mutex(pack, OutQueue, &queue, &mutex);

    while(queue_has_data)
    {
        DataTable tbl;

        mutex->lock();
        {
            if(queue->count() > 0)
                tbl = queue->dequeue();

            queue_has_data = queue->count() > 0;
        }
        mutex->unlock();

        QByteArray data = tbl.ToXmlQString().toAscii();

        if(_derived_class_pointer)
            _derived_class_pointer->preprocess_outgoing_data(data);

        try
        {
            transport().SendData(data);
        }
        catch(Core::Exception &ex)
        {
            Logging::GlobalLogger::LogException(ex);
            // Don't crash on transport errors
            //throw;
        }

        pack->ConditionDataWritten.wakeAll();
    }
}

void GIODeviceBundleManager::_receive_incoming_data(IODevicePackage *pack)
{
    QByteArray data;
    try
    {
        data = pack->IODevice->ReceiveData(false);
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return;
    }


    if(_derived_class_pointer)
        _derived_class_pointer->preprocess_incoming_data(data);

    bool exception_hit(false);
    DataTable tbl;
    try
    {
        tbl.FromXmlQString(data);
    }
    catch(Core::Exception &)
    {
        // Ignore and treat as if received nothing
        exception_hit = true;
    }

    if(!exception_hit)
    {
        pack->InQueueMutex.lock();
        {
            pack->InQueue.enqueue(tbl);
        }
        pack->InQueueMutex.unlock();

        emit NewDataArrived(pack->IODevice->GetIdentity());
    }
}

void GIODeviceBundleManager::_worker_outgoing(IODevicePackage *pack)
{
//    QMutex *flags_mutex(&pack->OutgoingFlagsMutex);
//    QWaitCondition *condition_data_ready(&pack->ConditionOutgoingDataEnqueued);
//    bool *flag_data_ready(&pack->FlagNewOutgoingDataEnqueued);

    pack->OutgoingFlagsMutex.lock();
    {
        forever
        {
            // Somebody may have queued more data for us while we
            //  were processing the last run.  If so, we skip waiting and
            //  go right to process the queue.
            while(!pack->FlagNewOutgoingDataEnqueued && !pack->FlagCancel)
                pack->ConditionOutgoingDataEnqueued.
                        wait(&pack->OutgoingFlagsMutex);

            if(pack->FlagCancel)
                break;

            // lower the flag
            pack->FlagNewOutgoingDataEnqueued = false;


            // Process any data in the queue (without holding the lock)
            pack->OutgoingFlagsMutex.unlock();

            _flush_out_queue(pack);

            pack->OutgoingFlagsMutex.lock();
        }
    }
    pack->OutgoingFlagsMutex.unlock();
}

void GIODeviceBundleManager::_worker_incoming(IODevicePackage *pack)
{
    pack->IncomingFlagsMutex.lock();
    {
        forever
        {
            while(!pack->FlagNewIncomingDataReady && !pack->FlagCancel)
                pack->ConditionIncomingDataReadyToRead.wait(&pack->IncomingFlagsMutex);

            if(pack->FlagCancel)
                break;

            pack->FlagNewIncomingDataReady = false;
            pack->IncomingFlagsMutex.unlock();

            _receive_incoming_data(pack);

            pack->IncomingFlagsMutex.lock();
        }
    }
    pack->IncomingFlagsMutex.unlock();
}

void GIODeviceBundleManager::wait_for_message_sent(const QUuid &msg_id,
                                                   const QUuid &device_id)
{
    IODevicePackage *pack(_get_package(device_id));

    pack->OutQueueMutex.lock();
    {
        bool contains(true);

        while(contains)
        {
            contains = false;

            // Figure out if the queue contains the given id
            foreach(DataTable tbl, pack->OutQueue)
            {
                if(msg_id == tbl.Name())
                {
                    contains = true;
                    break;
                }
            }

            if(contains)
                pack->ConditionDataWritten.wait(&pack->OutQueueMutex);
        }
    }
    pack->OutQueueMutex.unlock();
}

GIODeviceBundleManager::IODevicePackage *
        GIODeviceBundleManager::_get_package(const QUuid &id) const
{
    IODevicePackage *ret(0);
    if(id.isNull())
    {
        Q_ASSERT(_iodevices.count() > 0);
        ret = _iodevices.begin().value();
    }
    else
        ret = _iodevices[id];
    return ret;
}

QUuid GIODeviceBundleManager::SendData(const DataTable &t,
                                       const QUuid &id)
{
    IODevicePackage *pack(_get_package(id));
    QUuid ret(QUuid::createUuid());
    DataTable tmp(t.Clone());

    // Tag the table data with a GUID
    tmp.SetTableName(ret.toString());

    // enqueue the data
    pack->OutQueueMutex.lock();
    {
        pack->OutQueue.enqueue(tmp);
    }
    pack->OutQueueMutex.unlock();

    // Wake up our background worker to the fact that there's new
    //  data available.  If they miss the wakeup because they're already
    //  running, then they will see that _new_***_work_queued has
    //  been set to true, and they will reprocess the appropriate queue.
    pack->OutgoingFlagsMutex.lock();
    {
        pack->FlagNewOutgoingDataEnqueued = true;
    }
    pack->OutgoingFlagsMutex.unlock();

    pack->ConditionOutgoingDataEnqueued.wakeOne();

    // If we write synchronously, then we wait here until the data is actually written
    if(!pack->GetAsyncWrite())
        wait_for_message_sent(ret, id);

    return ret;
}

DataTable GIODeviceBundleManager::ReceiveData(const QUuid &id)
{
    IODevicePackage *pack(_get_package(id));
    DataTable ret;

    // en/dequeue the data
    pack->InQueueMutex.lock();
    {
        if(!pack->InQueue.empty())
            ret = pack->InQueue.dequeue();
    }
    pack->InQueueMutex.unlock();

    return ret;
}

bool GIODeviceBundleManager::HasData(const QUuid &id)
{
    return transport(id).HasDataAvailable();
}

void GIODeviceBundleManager::InsertIntoBundle(DataAccess::GIODevice *iodevice)
{
    QUuid id(iodevice->GetIdentity());
    if(_iodevices.contains(id))
        return;

    IODevicePackage *pack = new IODevicePackage(iodevice);

    _iodevices.insert(iodevice->GetIdentity(), pack);


    connect(iodevice, SIGNAL(ReadyRead(QUuid)),
            this, SLOT(importData(QUuid)));

    // Make sure we have enough threads to effectively use this device
    _current_threads += 2;
    if(_current_threads > QThreadPool::globalInstance()->maxThreadCount())
        QThreadPool::globalInstance()->setMaxThreadCount(_current_threads);

    // Start up our background workers for this device
    pack->WorkerOutgoing =
            QtConcurrent::run(this, &GIODeviceBundleManager::_worker_outgoing,
                              pack);

    pack->WorkerIncoming =
            QtConcurrent::run(this, &GIODeviceBundleManager::_worker_incoming,
                              pack);
}

void GIODeviceBundleManager::Remove(const QUuid &id)
{
    if(!_iodevices.contains(id))
        return;

    IODevicePackage *pack(_get_package(id));

    // First gracefully cancel the background threads
    pack->IncomingFlagsMutex.lock();
    pack->OutgoingFlagsMutex.lock();
    {
        pack->FlagCancel = true;
    }
    pack->OutgoingFlagsMutex.unlock();
    pack->IncomingFlagsMutex.unlock();

    pack->ConditionOutgoingDataEnqueued.wakeOne();
    pack->ConditionIncomingDataReadyToRead.wakeOne();

    pack->WorkerOutgoing.waitForFinished();
    pack->WorkerIncoming.waitForFinished();

    _current_threads -= 2;

    // Then disconnect and delete the io device
    disconnect(_iodevices[id]->IODevice, SIGNAL(ReadyRead(QUuid)),
               this, SLOT(importData(QUuid)));

    delete _iodevices[id];
    _iodevices.remove(id);
}
