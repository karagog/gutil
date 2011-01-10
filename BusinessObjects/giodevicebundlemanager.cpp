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
#include "Core/math.h"
#include <QStringList>
#include <QtConcurrentRun>
#include <QThreadPool>
using namespace GUtil;
using namespace Core;
using namespace DataObjects;
using namespace BusinessObjects;

#define MIN_THREADS 4

GIODeviceBundleManager::GIODeviceBundleManager(QObject *parent)
            :QObject(parent),
              flagCancelThread(false)
{
    _thread_pool.setExpiryTimeout(-1);
    _thread_pool.setMaxThreadCount( Max(MIN_THREADS, QThread::idealThreadCount()) );
}

GIODeviceBundleManager::~GIODeviceBundleManager()
{
    // Kill all the running threads
    RemoveAll(true);
}

// Wake up our background worker in charge of this ID
void GIODeviceBundleManager::importData(const QUuid &id)
{
    flagsMutex.lock();
    {
        work_queue.enqueue(WorkItem(id, WorkItem::Incoming));
    }
    flagsMutex.unlock();

    forSomethingToDo.wakeOne();
}

void GIODeviceBundleManager::WorkerThread::_write_one_packet(IODevicePackage *pack)
{
    QByteArray data;

    pack->OutQueueMutex.lock();
    {
        if(pack->OutQueue.count() > 0)
            data = pack->OutQueue.dequeue().second;
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

void GIODeviceBundleManager::WorkerThread::_receive_incoming_data(IODevicePackage *pack)
{
    if(pack->IODevice->HasDataAvailable())
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

        pack->InQueueMutex.lock();
        {
            pack->InQueue.enqueue(data);
        }
        pack->InQueueMutex.unlock();

        emit _bundle_manager->NewDataArrived(pack->IODevice->GetIdentity(),
                                             data);
    }
}

void GIODeviceBundleManager::WaitForMessageSent(const QUuid &msg_id,
                                                   const QUuid &device_id)
{
    IODevicePackage *pack(get_package(device_id));

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
        GIODeviceBundleManager::get_package(const QUuid &id) const
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
    IODevicePackage *pack(get_package(id));
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
    flagsMutex.lock();
    {
        work_queue.enqueue(WorkItem(id, WorkItem::Outgoing));
    }
    flagsMutex.unlock();

    forSomethingToDo.wakeOne();

    // If we write synchronously, then we wait here until the data is actually written
    if(!pack->GetAsyncWrite())
        WaitForMessageSent(ret, id);

    return ret;
}

QByteArray GIODeviceBundleManager::ReceiveData(const QUuid &id)
{
    IODevicePackage *pack(get_package(id));
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

bool GIODeviceBundleManager::HasData(const QUuid &id) const
{
    IODevicePackage *pack(get_package(id));
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

    // Allocate a new thread if we still have room for threads
    if(_iodevices.count() < _thread_pool.maxThreadCount())
        _thread_pool.start(new WorkerThread(this));

    IODevicePackage *pack = new IODevicePackage(iodevice);
    _iodevices.insert(iodevice->GetIdentity(), pack);

    connect(iodevice, SIGNAL(ReadyRead(QUuid)),
            this, SLOT(importData(QUuid)));
}

void GIODeviceBundleManager::RemoveAll(bool synchronous)
{
    foreach(QUuid id, _iodevices.keys())
        Remove(id);

    if(synchronous)
        _thread_pool.waitForDone();
}

void GIODeviceBundleManager::Remove(const QUuid &id)
{
    if(!_iodevices.contains(id))
        return;

    disconnect(_iodevices[id]->IODevice, SIGNAL(ReadyRead(QUuid)),
               this, SLOT(importData(QUuid)));

    delete _iodevices[id];
    _iodevices.remove(id);

    if(_iodevices.count() < _thread_pool.maxThreadCount())
    {
        // Cancel a background thead if there's not enough work to go around
        flagsMutex.lock();
        {
            flagCancelThread = true;
        }
        flagsMutex.unlock();

        forSomethingToDo.wakeOne();
    }
}

void GIODeviceBundleManager::WorkerThread::run()
{
    _bundle_manager->flagsMutex.lock();
    {
        // The forever breaks when the Cancel flag is set
        forever
        {
            // Somebody may have queued more data for us while we
            //  were processing the last run.  If so, we skip waiting and
            //  go right to process the queue.
            while(_bundle_manager->work_queue.isEmpty() &&
                  !_bundle_manager->flagCancelThread)
            {
                _bundle_manager->forSomethingToDo.wait(&_bundle_manager->flagsMutex);
            }

            if(_bundle_manager->flagCancelThread)
            {
                _bundle_manager->flagCancelThread = false;
                break;
            }

            if(!_bundle_manager->work_queue.isEmpty())
            {
                _bundle_manager->flagsMutex.unlock();

                GIODeviceBundleManager::WorkItem item(
                            _bundle_manager->work_queue.dequeue());

                GIODeviceBundleManager::IODevicePackage *pack(
                            _bundle_manager->get_package(item.Id));

                if(item.Direction == item.Outgoing)
                    _write_one_packet(pack);

                else if(item.Direction == item.Incoming)
                    _receive_incoming_data(pack);

                _bundle_manager->flagsMutex.lock();
            }
        }
    }
    _bundle_manager->flagsMutex.unlock();
}
