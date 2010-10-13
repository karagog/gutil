///*Copyright 2010 George Karagoulis

//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at

//    http://www.apache.org/licenses/LICENSE-2.0

//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.*/

//#include "localclientserver.h"
//#include "UserMachineLock.h"
//#include "Private/Transports/localsockettransport.h"
//#include "exception.h"
//#include <QXmlStreamWriter>
//#include <QXmlStreamReader>
//#include <QFile>
//#include <QThread>
//#include <QtConcurrentRun>
//using namespace GUtil::DataAccess;
//using namespace GUtil::DataAccess::Private;
//using namespace DA_LocalClientServer_Private;

//#define MODIFIER_STRING "localserver"


//DA_LocalSocket::DA_LocalSocket(const QString &identifier,
//                                                     const QString &modifier,
//                                                     QObject *parent)
//        : ValueBuffer(_transport = new LocalSocketTransport(new QLocalSocket(this)),
//                 parent)
//{
//    _machine_lock = new UserMachineLock(identifier, QString("%1.%2").arg(modifier).arg(MODIFIER_STRING), this);

//    _socket_identifier = QString("%1.%2").arg(identifier).arg(modifier);
//}

//DA_LocalSocket::~DA_LocalSocket()
//{
//}

//QString DA_LocalSocket::socketIdentifier() const
//{
//    return _socket_identifier;
//}

//QUuid DA_LocalSocket::createNewSocketWorker(QLocalSocket *newsock)
//{
//    if(newsock == 0)
//        return QUuid();

//    IO_Queue_Manager *sw = new IO_Queue_Manager(newsock, this);

//    connect(sw, SIGNAL(notifyNewMessageArrived(int, QByteArray)),
//            this, SLOT(messageArrived(int, QByteArray)));

//    worker_list.insert(sw->uniqueId(), sw);
//    return sw->uniqueId();
//}

//void DA_LocalSocket::sendMessage(QUuid worker_id, const QList<parameter_t> &params)
//{
//    if(worker_id.isNull())
//    {
//        // For this special case, broadcast the message to everybody listening
//        foreach(QPointer<IO_Queue_Manager> sw, worker_list.values())
//            _send_message(sw, params);
//    }
//    else
//    {
//        _send_message(worker_list.value(worker_id), params);
//    }
//}

//void DA_LocalSocket::_send_message(QPointer<IO_Queue_Manager> sw, const QList<parameter_t> &params)
//{
//    if(sw == 0)
//        return;

//    sw->enQueueMessage(IO_Queue_Manager::OutQueue, _convert_parameter_list_to_message(params));
//}

//void DA_LocalSocket::messageArrived(QUuid id, const QByteArray &ba)
//{
//    QList<parameter_t> tmpparms = _extract_parameter_list_from_message(ba);

//    for(int i = tmpparms.length(); i >= 0; i--)
//    {
//        parameter_t p = tmpparms.at(i);
//        tmpparms.removeAt(i);

//        if(p.isControlParameter)
//            _process_control_parameter(id, p);
//    }

//    if(tmpparms.length() > 0)
//        messageArrived(id, tmpparms);
//}

//void DA_LocalSocket::_process_control_parameter(QUuid id, parameter_t p)
//{
//    if(p.name.toUpper() == "GOODBYE")
//    {
//        // Close the socket and delete it

//        QPointer<IO_Queue_Manager> tmp = worker_list.value(id);
//        if(tmp != 0)
//        {
//            tmp->closeSocketConnection();

//            // Because it's a QPointer, it will automatically reset to 0
//            delete tmp;
//        }
//        worker_list.remove(id);

//        emit notifyConnectionClosed(id);
//    }
//}

//void DA_LocalSocket::closeConnection()
//{
//    sendControlSignal(QUuid(), GOODBYE);
//}

//void DA_LocalSocket::sendControlSignal(QUuid send_id, ControlSignal cs)
//{
//    parameter_t p;
//    p.isControlParameter = true;

//    switch(cs)
//    {
//    case NOOP:
//        p.name = "NOOP";
//        break;
//    case GOODBYE:
//        p.name = "BYE";
//        break;
//    default:
//        return;
//    }

//    QList<parameter_t> args;
//    args.append(p);

//    sendMessage(send_id, args);
//}

//void DA_LocalSocket::broadcastMessage(const QList<parameter_t> &params)
//{
//    sendMessage(QUuid(), params);
//}

//QList<DA_LocalSocket::parameter_t> DA_LocalSocket::
//        _extract_parameter_list_from_message(const QByteArray &data)
//{
//    QList<parameter_t> params;
//    QXmlStreamReader sr(data);

//    // Read in the start document
//    sr.readNextStartElement();

//    // Then the first element
//    sr.readNextStartElement();

//    while(sr.readNextStartElement())
//    {
//        parameter_t tmpparm;

//        foreach(QXmlStreamAttribute sa, sr.attributes())
//        {
//            QString s = sa.name().toString();
//            char c = s.length() > 0 ? s.at(0).toAscii() : '\0';
//            switch(c)
//            {
//            case 'n':
//                tmpparm.name = sa.value().toString();
//                break;
//            case 'v':
//                tmpparm.value = sa.value().toString();
//                break;
//            case 'c':
//                tmpparm.isControlParameter = true;
//                break;
//            default:
//                break;
//            }
//        }

//        params.append(tmpparm);

//        // Read in the end of the element
//        while(sr.readNext() != QXmlStreamReader::EndElement);
//    }

//    return params;
//}

//QByteArray DA_LocalSocket::_convert_parameter_list_to_message(const QList<parameter_t> &params)
//{
//    QByteArray xml;
//    QXmlStreamWriter sw(&xml);
//    sw.setAutoFormatting(false);
//    sw.writeStartDocument();

//    sw.writeStartElement("message");
//    foreach(parameter_t p, params)
//    {
//        sw.writeStartElement("p");

//        if(p.isControlParameter)
//            sw.writeAttribute("c", "");

//        sw.writeAttribute("n", p.name);
//        sw.writeAttribute("v", p.value);
//        sw.writeEndElement();
//    }
//    sw.writeEndElement();
//    sw.writeEndDocument();

//    return xml;
//}








//DA_LocalServer::DA_LocalServer(const QString &identifier, const QString &modifier, QObject *parent)
//    :DA_LocalSocket(identifier, modifier, parent)
//{
//    // The server must have an exclusive lock, otherwise we cede control
//    _machine_lock->lock();

//    if(hasExclusiveLock())
//    {
//        // Initialize the server
//        if(!_server.listen(socketIdentifier()))
//        {
//            if(!_server.removeServer(socketIdentifier()))
//                throw GUtil::Exception(_server.errorString().toStdString());

//            if(!_server.listen(socketIdentifier()))
//                throw GUtil::Exception(_server.errorString().toStdString());
//        }

//        connect(&_server, SIGNAL(newConnection()), this, SLOT(new_connection()));
//    }
//}

//void DA_LocalServer::new_connection()
//{
//    while(_server.hasPendingConnections())
//        emit notifyNewConnection(createNewSocketWorker(_server.nextPendingConnection()));
//}









//DA_LocalClient::DA_LocalClient(const QString &identifier, const QString &modifier, QObject *parent)
//    :DA_LocalSocket(identifier, modifier, parent)
//{
//    // Initialize the client
//    QLocalSocket *tmpsock = new QLocalSocket(this);
//    tmpsock->connectToServer(socketIdentifier());
//    if(!tmpsock->waitForConnected(5000))
//        throw GUtil::Exception(tmpsock->errorString().toStdString());

//    createNewSocketWorker(tmpsock);
//}

//void DA_LocalClient::sendMessage(const QList<parameter_t> &params)
//{
//    DA_LocalSocket::sendMessage(QUuid(), params);
//}
