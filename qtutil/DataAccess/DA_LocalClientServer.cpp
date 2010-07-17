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

#include "da_localclientserver.h"
#include "DA_UserMachineLock.h"
#include "Private/Transports/sockettransport.h"
#include "exception.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QFile>
#include <QThread>
#include <QtConcurrentRun>
using namespace GQtUtil::DataAccess;
using namespace GQtUtil::DataAccess::Private;
using namespace DA_LocalClientServer_Private;

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace DA_LocalClientServer_Private
        {
            // You use this class as a "data access" layer for socket data.  You can queue
            //   and dequeue messages and it will take care of sending and notifying
            //   you when there's a response
            class socket_worker : public QObject
            {
                Q_OBJECT
            public:
                QUuid uniqueId() const;

                socket_worker(QLocalSocket *, QObject *parent = 0);
                ~socket_worker();

                enum QueueType
                {
                    InQueue,
                    OutQueue
                };

                // Use this to safely remove an item from the in_queue
                QByteArray deQueueMessage(QueueType);
                void enQueueMessage(QueueType, const QByteArray &);

                void closeSocketConnection();

            signals:
                // This signal is emitted for every new message found in the in_queue
                void notifyNewMessageArrived(QUuid, QByteArray);

            private:
                QUuid _id;
                QMutex socket_mutex;
                QMutex in_queue_mutex;
                QMutex out_queue_mutex;
                QLocalSocket *_sock;
                QQueue< QByteArray > in_queue;
                QQueue< QByteArray > out_queue;

                void receive_socket_data();

            private slots:

                void process_queues();
                void flush_input_queue();
                void flush_output_queue();

                void socket_data_ready();
                void write_output();
            };

            socket_worker::socket_worker(QLocalSocket *sock, QObject *parent)
                :QObject(parent)
            {
                _id = QUuid::createUuid();
                _sock = sock;

                // Re-parent the socket so we'll delete it when we die
                _sock->setParent(this);

                // We make these queued connections to ensure that the socket is accessed only from
                //   the event thread
                connect(_sock, SIGNAL(readyRead()), this, SLOT(socket_data_ready()));
            }

            socket_worker::~socket_worker()
            {
            }

            QUuid socket_worker::uniqueId() const
            {
                return _id;
            }

            void socket_worker::process_queues()
            {
                // Flush the queues
                flush_output_queue();

                flush_input_queue();
            }

            void socket_worker::enQueueMessage(QueueType q, const QByteArray &msg)
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

            QByteArray socket_worker::deQueueMessage(QueueType q)
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

            void socket_worker::flush_input_queue()
            {
                QByteArray params;

                while((params = deQueueMessage(InQueue)).length() > 0)
                    emit notifyNewMessageArrived(_id, params);
            }

            void socket_worker::flush_output_queue()
            {
                QtConcurrent::run(this, &socket_worker::write_output);
            }

            void socket_worker::socket_data_ready()
            {
                QtConcurrent::run(this, &socket_worker::receive_socket_data);
            }

            void socket_worker::receive_socket_data()
            {
                socket_mutex.lock();

                while(_sock->bytesAvailable() > 0)
                    enQueueMessage(InQueue, _sock->readAll());

                socket_mutex.unlock();
            }

            void socket_worker::write_output()
            {
                QByteArray data;

                while((data = deQueueMessage(OutQueue)).length() > 0)
                {
                    socket_mutex.lock();
                    _sock->write(data);
                    socket_mutex.unlock();
                }
            }

            void socket_worker::closeSocketConnection()
            {
                _sock->close();
            }
        }
    }
}




#define MODIFIER_STRING "localserver"


DA_LocalClientServer_Base::DA_LocalClientServer_Base(const QString &identifier,
                                                     const QString &modifier,
                                                     QObject *parent):
DA_ValueBuffer_P(_transport = new Transports::SocketTransport(new QLocalSocket(this)),
                 parent)
{
    _machine_lock = new DA_UserMachineLock(identifier, QString("%1.%2").arg(modifier).arg(MODIFIER_STRING), this);

    _socket_identifier = QString("%1.%2").arg(identifier).arg(modifier);
}

DA_LocalClientServer_Base::~DA_LocalClientServer_Base()
{
//    foreach(socket_worker *sw, worker_list)
//        sw->terminate();
}

QString DA_LocalClientServer_Base::socketIdentifier() const
{
    return _socket_identifier;
}

QUuid DA_LocalClientServer_Base::createNewSocketWorker(QLocalSocket *newsock)
{
    if(newsock == 0)
        return QUuid();

    socket_worker *sw = new socket_worker(newsock, this);

    connect(sw, SIGNAL(notifyNewMessageArrived(int, QByteArray)),
            this, SLOT(messageArrived(int, QByteArray)));

    worker_list.insert(sw->uniqueId(), sw);
    return sw->uniqueId();
}

void DA_LocalClientServer_Base::sendMessage(QUuid worker_id, const QList<parameter_t> &params)
{
    if(worker_id.isNull())
    {
        // For this special case, broadcast the message to everybody listening
        foreach(QPointer<socket_worker> sw, worker_list.values())
            _send_message(sw, params);
    }
    else
    {
        _send_message(worker_list.value(worker_id), params);
    }
}

void DA_LocalClientServer_Base::_send_message(QPointer<socket_worker> sw, const QList<parameter_t> &params)
{
    if(sw == 0)
        return;

    sw->enQueueMessage(socket_worker::OutQueue, _convert_parameter_list_to_message(params));
}

void DA_LocalClientServer_Base::messageArrived(QUuid id, const QByteArray &ba)
{
    QList<parameter_t> tmpparms = _extract_parameter_list_from_message(ba);

    for(int i = tmpparms.length(); i >= 0; i--)
    {
        parameter_t p = tmpparms.at(i);
        tmpparms.removeAt(i);

        if(p.isControlParameter)
            _process_control_parameter(id, p);
    }

    if(tmpparms.length() > 0)
        messageArrived(id, tmpparms);
}

void DA_LocalClientServer_Base::_process_control_parameter(QUuid id, parameter_t p)
{
    if(p.name.toUpper() == "GOODBYE")
    {
        // Close the socket and delete it

        QPointer<socket_worker> tmp = worker_list.value(id);
        if(tmp != 0)
        {
            tmp->closeSocketConnection();

            // Because it's a QPointer, it will automatically reset to 0
            delete tmp;
        }
        worker_list.remove(id);

        emit notifyConnectionClosed(id);
    }
}

void DA_LocalClientServer_Base::closeConnection()
{
    sendControlSignal(QUuid(), GOODBYE);
}

void DA_LocalClientServer_Base::sendControlSignal(QUuid send_id, ControlSignal cs)
{
    parameter_t p;
    p.isControlParameter = true;

    switch(cs)
    {
    case NOOP:
        p.name = "NOOP";
        break;
    case GOODBYE:
        p.name = "BYE";
        break;
    default:
        return;
    }

    QList<parameter_t> args;
    args.append(p);

    sendMessage(send_id, args);
}

void DA_LocalClientServer_Base::broadcastMessage(const QList<parameter_t> &params)
{
    sendMessage(QUuid(), params);
}

QList<DA_LocalClientServer_Base::parameter_t> DA_LocalClientServer_Base::
        _extract_parameter_list_from_message(const QByteArray &data)
{
    QList<parameter_t> params;
    QXmlStreamReader sr(data);

    // Read in the start document
    sr.readNextStartElement();

    // Then the first element
    sr.readNextStartElement();

    while(sr.readNextStartElement())
    {
        parameter_t tmpparm;

        foreach(QXmlStreamAttribute sa, sr.attributes())
        {
            QString s = sa.name().toString();
            char c = s.length() > 0 ? s.at(0).toAscii() : '\0';
            switch(c)
            {
            case 'n':
                tmpparm.name = sa.value().toString();
                break;
            case 'v':
                tmpparm.value = sa.value().toString();
                break;
            case 'c':
                tmpparm.isControlParameter = true;
                break;
            default:
                break;
            }
        }

        params.append(tmpparm);

        // Read in the end of the element
        while(sr.readNext() != QXmlStreamReader::EndElement);
    }

    return params;
}

QByteArray DA_LocalClientServer_Base::_convert_parameter_list_to_message(const QList<parameter_t> &params)
{
    QByteArray xml;
    QXmlStreamWriter sw(&xml);
    sw.setAutoFormatting(false);
    sw.writeStartDocument();

    sw.writeStartElement("message");
    foreach(parameter_t p, params)
    {
        sw.writeStartElement("p");

        if(p.isControlParameter)
            sw.writeAttribute("c", "");

        sw.writeAttribute("n", p.name);
        sw.writeAttribute("v", p.value);
        sw.writeEndElement();
    }
    sw.writeEndElement();
    sw.writeEndDocument();

    return xml;
}








DA_LocalServer::DA_LocalServer(const QString &identifier, const QString &modifier, QObject *parent)
    :DA_LocalClientServer_Base(identifier, modifier, parent)
{
    // The server must have an exclusive lock, otherwise we cede control
    _machine_lock->lock();

    if(hasExclusiveLock())
    {
        // Initialize the server
        if(!_server.listen(socketIdentifier()))
        {
            if(!_server.removeServer(socketIdentifier()))
                throw GUtil::Exception(_server.errorString().toStdString());

            if(!_server.listen(socketIdentifier()))
                throw GUtil::Exception(_server.errorString().toStdString());
        }

        connect(&_server, SIGNAL(newConnection()), this, SLOT(new_connection()));
    }
}

void DA_LocalServer::new_connection()
{
    while(_server.hasPendingConnections())
        emit notifyNewConnection(createNewSocketWorker(_server.nextPendingConnection()));
}









DA_LocalClient::DA_LocalClient(const QString &identifier, const QString &modifier, QObject *parent)
    :DA_LocalClientServer_Base(identifier, modifier, parent)
{
    // Initialize the client
    QLocalSocket *tmpsock = new QLocalSocket(this);
    tmpsock->connectToServer(socketIdentifier());
    if(!tmpsock->waitForConnected(5000))
        throw GUtil::Exception(tmpsock->errorString().toStdString());

    createNewSocketWorker(tmpsock);
}

void DA_LocalClient::sendMessage(const QList<parameter_t> &params)
{
    DA_LocalClientServer_Base::sendMessage(QUuid(), params);
}
