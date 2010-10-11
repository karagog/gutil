#include "io_queue_worker.h"
#include <QtConcurrentRun>

using namespace GUtil::DataAccess::Private;

IO_Queue_Worker::IO_Queue_Worker(QIODevice *io_device, QObject *parent)
    :QObject(parent)
{
    _id = QUuid::createUuid();

    _io_device = io_device;

    // Re-parent the socket so we'll delete it when we die
    _io_device->setParent(this);

    // We make these queued connections to ensure that the socket is accessed only from
    //   the event thread
    connect(_io_device, SIGNAL(readyRead()), this, SLOT(socket_data_ready()));
}

IO_Queue_Worker::~IO_Queue_Worker()
{
}

QUuid IO_Queue_Worker::uniqueId() const
{
    return _id;
}

void IO_Queue_Worker::process_queues()
{
    // Flush the queues
    flush_output_queue();

    flush_input_queue();
}

void IO_Queue_Worker::enQueueMessage(QueueType q, const QByteArray &msg)
{
    QMutex *m;
    QQueue< QByteArray > *tmpq;

    switch(q)
    {
    case InQueue:
        m = &in_queue_mutex;
        tmpq = &in_queue;
        break;
    case OutQueue:
        m = &out_queue_mutex;
        tmpq = &out_queue;
        break;
    default:
        return;
    }

    m->lock();
    tmpq->enqueue(msg);
    m->unlock();

    // We want to get rid of anything in the queue as soon as possible
    process_queues();
}

QByteArray IO_Queue_Worker::deQueueMessage(QueueType q)
{
    QByteArray ret;
    QMutex *m;
    QQueue<QByteArray> *tmpq;

    switch(q)
    {
    case InQueue:
        m = &in_queue_mutex;
        tmpq = &in_queue;
        break;
    case OutQueue:
        m = &out_queue_mutex;
        tmpq = &out_queue;
        break;
    default:
        return ret;
    }

    m->lock();
    if(!tmpq->empty())
        ret = tmpq->dequeue();
    m->unlock();

    return ret;
}

void IO_Queue_Worker::flush_input_queue()
{
    QByteArray params;

    while((params = deQueueMessage(InQueue)).length() > 0)
        emit notifyNewMessageArrived(_id, params);
}

void IO_Queue_Worker::flush_output_queue()
{
    QtConcurrent::run(this, &IO_Queue_Worker::write_output);
}

void IO_Queue_Worker::socket_data_ready()
{
    QtConcurrent::run(this, &IO_Queue_Worker::receive_socket_data);
}

void IO_Queue_Worker::receive_socket_data()
{
    io_mutex.lock();

    while(_io_device->bytesAvailable() > 0)
        enQueueMessage(InQueue, _io_device->readAll());

    io_mutex.unlock();
}

void IO_Queue_Worker::write_output()
{
    QByteArray data;

    while((data = deQueueMessage(OutQueue)).length() > 0)
    {
        io_mutex.lock();
        _io_device->write(data);
        io_mutex.unlock();
    }
}

void IO_Queue_Worker::closeSocketConnection()
{
    _io_device->close();
}
