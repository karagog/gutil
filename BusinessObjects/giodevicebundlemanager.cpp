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
using namespace GUtil;
using namespace DataObjects;
using namespace BusinessObjects;

GIODeviceBundleManager::GIODeviceBundleManager(
        DataAccess::GIODevice *transport,
        DerivedClassFunctions *dp,
        QObject *parent)
            :QObject(parent),
            _flag_new_outgoing_data_enqueued(false),
            _flag_new_incoming_data_enqueued(false),
            _flag_exiting(false),
            _transport(transport),
            _derived_class_pointer(dp)
{
    start_worker_threads();
}

GIODeviceBundleManager::~GIODeviceBundleManager()
{
    // Derived classes must call this first thing in their destructors, but just
    //  in case we still call it here.  But this may cause a seg fault if called
    //  here
    //kill_worker_threads();
}

void GIODeviceBundleManager::kill_worker_threads()
{
    _incoming_flags_mutex.lock();
    _outgoing_flags_mutex.lock();
    {
        _flag_exiting = true;
    }
    _outgoing_flags_mutex.unlock();
    _incoming_flags_mutex.unlock();

    // Wake up our background workers and wait for them to finish
    _condition_outgoing_data_enqueued.wakeOne();
    _condition_incoming_data_enqueued.wakeOne();

    _ref_worker_incoming.waitForFinished();
    _ref_worker_outgoing.waitForFinished();
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
    IODevicePackage *pack(_get_package(id);

    pack->IncomingFlagsMutex.lock();
    {
        pack->FlagNewIncomingDataReady = true;
    }
    pack->IncomingFlagsMutex.unlock();

    pack->ConditionIncomingDataReadyToRead.wakeOne();
}

DataTable GIODeviceBundleManager::deQueueMessage(QueueTypeEnum q,
                                                 const QUuid &id)
{
    return en_deQueueMessage(q, DataTable(), false, id);
}

DataTable GIODeviceBundleManager::en_deQueueMessage(
        QueueTypeEnum q,
        const DataTable &msg,
        bool enqueue,
        const QUuid &id)
{
    IODevicePackage *pack(_get_package(id));
    DataTable *tmp_tbl;
    QMutex *queue_mutex;
    QQueue<DataTable> *queue;

    bool *flag_new_data;
    QMutex *flags_mutex;
    QWaitCondition *condition_newdata_available;

    try
    {
        _get_queue_and_mutex(pack, q, &queue, &queue_mutex);
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return DataTable();
    }

    // Set the wait condition and the new data flags
    switch(q)
    {
    case InQueue:
        flag_new_data = &_flag_new_incoming_data_enqueued;
        flags_mutex = &_incoming_flags_mutex;
        condition_newdata_available = &_condition_incoming_data_enqueued;
        break;
    case OutQueue:
        flag_new_data = &_flag_new_outgoing_data_enqueued;
        flags_mutex = &_outgoing_flags_mutex;
        condition_newdata_available = &_condition_outgoing_data_enqueued;
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION2(Core::NotImplementedException, "");
    }

    // en/dequeue the data
    queue_mutex->lock();
    {
        if(enqueue)
            queue->enqueue(*(tmp_tbl = new DataTable(msg.Clone())));
        else if(!queue->empty())
            tmp_tbl = new DataTable(queue->dequeue());
    }
    queue_mutex->unlock();

    // Wake up our background worker to the fact that there's new
    //  data available.  If they miss the wakeup because they're already
    //  running, then they will see that _new_***_work_queued has
    //  been set to true, and they will reprocess the appropriate queue.
    if(enqueue)
    {
        flags_mutex->lock();
        {
            *flag_new_data = true;
        }
        flags_mutex->unlock();

        condition_newdata_available->wakeOne();
    }

    // Create a return table on the stack.  We do this because creating a
    //  DataTable using the copy constructor is significantly less time consuming
    //  than creating it with the default constructor.  It is a shared data class
    //  so when copying all it does is set a single pointer, but when creating
    //  from scratch it would allocate a new shared data structure only to
    //  delete it when you use the '=' operator to set the return data
    DataTable ret(*tmp_tbl);
    delete tmp_tbl;

    // If we write synchronously, then we wait here until the data is actually written
    if(!GetAsyncWrite())
        wait_for_message_sent(ret.Name());

    return ret;
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

        _condition_outgoing_data_sent.wakeAll();
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

    pack->OutgoingFlagsMutex->lock();
    {
        forever
        {
            // Somebody may have queued more data for us while we
            //  were processing the last run.  If so, we skip waiting and
            //  go right to process the queue.
            while(!pack->FlagNewOutgoingDataEnqueued && !_flag_exiting)
                pack->ConditionOutgoingDataEnqueued->
                        wait(&pack->OutgoingFlagsMutex);

            if(_flag_exiting)
                break;

            // lower the flag
            pack->FlagNewOutgoingDataEnqueued = false;


            // Process any data in the queue (without holding the lock)
            pack->OutgoingFlagsMutex->unlock();

            _flush_out_queue(pack);

            pack->OutgoingFlagsMutex->lock();
        }
    }
    pack->OutgoingFlagsMutex->unlock();
}

void GIODeviceBundleManager::_worker_incoming(IODevicePackage *pack)
{
    pack->IncomingFlagsMutex.lock();
    {
        forever
        {
            while(!pack->FlagNewIncomingDataReady && !_flag_exiting)
                pack->ConditionIncomingDataReadyToRead.wait(&pack);

            if(_flag_exiting)
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
                _condition_outgoing_data_sent.wait(&pack->OutQueueMutex);
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

QUuid GIODeviceBundleManager::SendData(const QUuid &id, const DataTable &t)
{
    IODevicePackage *pack(_iodevices[id]);
    QUuid ret(QUuid::createUuid());
    DataTable tmp(t.Clone());

    // Tag the table data with a GUID
    tmp.SetTableName(ret.toString());

    en_deQueueMessage(pack, OutQueue, tmp, true);

    return ret;
}

DataTable GIODeviceBundleManager::ReceiveData(const QUuid &)
{
    QByteArray *data;

    try
    {
        // First get the data off the io device
        data = new QByteArray(transport(id).ReceiveData(false));
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return;
    }


    // Translate, if necessary
    if(_derived_class_pointer)
        _derived_class_pointer->preprocess_incoming_data(*data);


    DataTable tbl;
    try
    {
        // Populate the table with the xml data
        tbl.FromXmlQString(*data);
    }
    catch(Core::XmlException &)
    {
        // If the xml parse fails, then it's corrupt for whatever reason.
        //  we'll ignore it and treat it as a null configuration
        tbl.Clear();

        if(_derived_class_pointer)
            _derived_class_pointer->init_new_table(tbl);
    }

    // Enqueue it
    en_deQueueMessage(InQueue, tbl, true);
    delete data;
}

bool GIODeviceBundleManager::HasData(const QUuid &id)
{
    return _iodevices[id]->IODevice->HasDataAvailable();
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
    pack->WorkerOutgoing =
            QtConcurrent::run(this, &GIODeviceBundleManager::_worker_outgoing,
                              pack);

    pack->WorkerIncoming =
            QtConcurrent::run(this, &GIODeviceBundleManager::_worker_incoming,
                              pack);
}

void GIODeviceBundleManager::Remove(const QUuid &id)
{
    if(_iodevices.contains(id))
    {
        disconnect(_iodevices[id]->IODevice, SIGNAL(ReadyRead(QUuid)),
                   this, SLOT(importData(QUuid)));

        delete _iodevices[id];
        _iodevices.remove(id);
    }
}
