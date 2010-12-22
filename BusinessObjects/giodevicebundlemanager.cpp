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

GIODeviceBundleManager::GIODeviceBundleManager(QObject *parent)
            :QObject(parent)
{}

GIODeviceBundleManager::~GIODeviceBundleManager()
{
    // Kill all the running threads
    RemoveAll();
}

void GIODeviceBundleManager::RemoveAll()
{
    // Loop through and cancel all the threads
    foreach(QUuid id, _iodevices.keys())
    {
        IODevicePackage *pack = _iodevices[id];

        pack->FlagsMutex.lock();
        {
            pack->FlagCancel = true;
        }
        pack->FlagsMutex.unlock();

        pack->ForSomethingToDo.wakeOne();
    }

    // Now wait for them to finish
    foreach(QUuid id, _iodevices.keys())
    {
        IODevicePackage *pack = _iodevices[id];

        pack->Worker.waitForFinished();
        Remove(id);
    }
}

// Wake up our background worker in charge of this ID
void GIODeviceBundleManager::importData(const QUuid &id)
{
    IODevicePackage *pack(_get_package(id));

    pack->FlagsMutex.lock();
    {
        pack->FlagNewIncomingDataReady = true;
    }
    pack->FlagsMutex.unlock();

    pack->ForSomethingToDo.wakeOne();
}

void GIODeviceBundleManager::_flush_out_queue(IODevicePackage *pack)
{
    bool queue_has_data(true);

    while(queue_has_data)
    {
        QByteArray data;

        pack->OutQueueMutex.lock();
        {
            if(pack->OutQueue.count() > 0)
                data = pack->OutQueue.dequeue().second;

            queue_has_data = pack->OutQueue.count() > 0;
        }
        pack->OutQueueMutex.unlock();

        try
        {
            pack->IODevice->SendData(data);
        }
        catch(Core::Exception &ex)
        {
            Logging::GlobalLogger::LogException(ex);
            // Don't crash on transport errors
            //throw;
        }

        pack->ForDataWritten.wakeAll();
    }
}

void GIODeviceBundleManager::_receive_incoming_data(IODevicePackage *pack)
{
    while(pack->IODevice->HasDataAvailable())
    {
        QByteArray *data;

        try
        {
            data = new QByteArray(pack->IODevice->ReceiveData(false));
        }
        catch(Core::Exception &ex)
        {
            Logging::GlobalLogger::LogException(ex);
            return;
        }

        pack->InQueueMutex.lock();
        {
            pack->InQueue.enqueue(*data);
        }
        pack->InQueueMutex.unlock();

        delete data;

        emit NewDataArrived(pack->IODevice->GetIdentity());
    }
}

void GIODeviceBundleManager::_worker_thread(IODevicePackage *pack)
{
    pack->FlagsMutex.lock();
    {
        // The forever breaks when the Cancel flag is set
        forever
        {
            // Somebody may have queued more data for us while we
            //  were processing the last run.  If so, we skip waiting and
            //  go right to process the queue.
            while(!pack->FlagNewOutgoingDataEnqueued &&
                  !pack->FlagNewIncomingDataReady &&
                  !pack->FlagCancel)
            {
                pack->ForSomethingToDo.wait(&pack->FlagsMutex);
            }

            if(pack->FlagCancel)
                break;

            bool data_out(pack->FlagNewOutgoingDataEnqueued);
            bool data_in(pack->FlagNewIncomingDataReady);

            // lower the flags, 'cause we're about to address the work
            pack->FlagNewOutgoingDataEnqueued = false;
            pack->FlagNewIncomingDataReady = false;

            pack->FlagsMutex.unlock();
            {
                // NON-Critical/Work section; don't hold the flags lock while
                //  we do the background work

                // Design decision: send data before receiving data.
                //  The methodology is that other people waiting for us
                //  should get their data 'cause we're ready to send it,
                //  and then we have time to think about some new job
                //  coming in off the wire.
                if(data_out)
                    _flush_out_queue(pack);

                if(!pack->GetDropInput() && data_in)
                    _receive_incoming_data(pack);
            }
            pack->FlagsMutex.lock();
        }
    }
    pack->FlagsMutex.unlock();
}

void GIODeviceBundleManager::WaitForMessageSent(const QUuid &msg_id,
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
            for(int i = 0; i < pack->OutQueue.count(); i++)
            {
                if(msg_id == pack->OutQueue[i].first)
                {
                    contains = true;
                    break;
                }
            }

            if(contains)
                pack->ForDataWritten.wait(&pack->OutQueueMutex);
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

QUuid GIODeviceBundleManager::SendData(const QByteArray &data,
                                       const QUuid &id)
{
    IODevicePackage *pack(_get_package(id));
    QUuid ret(QUuid::createUuid());

    // enqueue the data
    pack->OutQueueMutex.lock();
    {
        pack->OutQueue.enqueue(QPair<QUuid, QByteArray>(ret, data));
    }
    pack->OutQueueMutex.unlock();

    // Wake up our background worker to the fact that there's new
    //  data available.  If they miss the wakeup because they're already
    //  running, then they will see that _new_***_work_queued has
    //  been set to true, and they will reprocess the appropriate queue.
    pack->FlagsMutex.lock();
    {
        pack->FlagNewOutgoingDataEnqueued = true;
    }
    pack->FlagsMutex.unlock();

    pack->ForSomethingToDo.wakeOne();

    // If we write synchronously, then we wait here until the data is actually written
    if(!pack->GetAsyncWrite())
        WaitForMessageSent(ret, id);

    return ret;
}

QByteArray GIODeviceBundleManager::ReceiveData(const QUuid &id)
{
    IODevicePackage *pack(_get_package(id));
    QByteArray ret;

    // dequeue the data
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
    IODevicePackage *pack(_get_package(id));
    bool ret;

    pack->InQueueMutex.lock();
    {
        ret = !pack->InQueue.isEmpty();
    }
    pack->InQueueMutex.unlock();

    return ret;
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

    // Start up our background workers for this device
    pack->Worker = QtConcurrent::run(
            this, &GIODeviceBundleManager::_worker_thread, pack);
}

void GIODeviceBundleManager::Remove(const QUuid &id)
{
    if(!_iodevices.contains(id))
        return;

    IODevicePackage *pack(_get_package(id));

    // First gracefully cancel the background threads
    pack->FlagsMutex.lock();
    {
        pack->FlagCancel = true;
    }
    pack->FlagsMutex.unlock();

    pack->ForSomethingToDo.wakeOne();


    pack->Worker.waitForFinished();

    // Then disconnect and delete the io device
    disconnect(_iodevices[id]->IODevice, SIGNAL(ReadyRead(QUuid)),
               this, SLOT(importData(QUuid)));

    delete _iodevices[id];
    _iodevices.remove(id);
}
