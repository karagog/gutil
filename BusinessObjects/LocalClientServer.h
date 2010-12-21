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

//#ifndef DA_LOCALCLIENTSERVER_H
//#define DA_LOCALCLIENTSERVER_H

//#include "giodevicebundlemanager.h"
//#include <QString>
//#include <QList>
//#include <QMap>
//#include <QLocalServer>
//#include <QLocalSocket>
//#include <QFuture>
//#include <QQueue>
//#include <QUuid>
//#include <QMutex>
//#include <QPointer>

//namespace GUtil
//{
//    namespace BusinessObjects
//    {
//        class UserMachineLock;
//        class parameter_t
//        {
//        public:
//            parameter_t(){}
//        };

//        // This class is used as a low-level data transport between a socket client and a server.

//        // You are expected to derive from this class and use the protected methods
//        //   'sendMessage' and 'messageArrived' to pass messages between endpoints.

//        // Terminate the session on either the client or server with the 'goodbye' function

//        // The base class for both client and server endpoints (abstract)
//        class LocalClientServer : public GIODeviceBundleManager
//        {
//            Q_OBJECT
//        signals:
//            // This signal is emitted when our partner endpoint closes the connection with us
//            void notifyConnectionClosed(QUuid);

//        protected:

//            LocalClientServer(const QString &, const QString &, QObject *parent = 0);
//            ~LocalClientServer();

//            void sendMessage(QUuid, const QList<parameter_t> &params);
//            void broadcastMessage(const QList<parameter_t> &);
//            virtual void messageArrived(QUuid, const QList<parameter_t> &) = 0;
//            virtual void closeConnection();

//            enum ControlSignal
//            {
//                NOOP,
//                GOODBYE
//            };

//            void sendControlSignal(QUuid, ControlSignal);

//            // Call this to create a new socket endpoint.  Remember the ID for reference later
//            QUuid createNewSocketWorker(QLocalSocket *);

//            UserMachineLock *_machine_lock;

//            QString socketIdentifier() const;

//            Private::Transports::IODevice_Transportort *_transport;

//        private slots:
//            void messageArrived(QUuid, const QByteArray &);

//        private:
//            void _send_message(QPointer<DA_LocalClientServer_Private::IO_Queue                             const QList<parameter_t> &);
//            void _process_control_parameter(QUuid, parameter_t);

//            // Keep track of a list of socket workers
//            QMap< QUuid, QPointer< DA_LocalClientServer_Private::IO_Queueworker_list;

//            static QList<parameter_t> _extract_parameter_list_from_message(const QByteArray &);
//            static QByteArray _convert_parameter_list_to_message(const QList<parameter_t>&);

//            QString _socket_identifier;
//        };


//        // Use this class as the server endpoint
//        class DA_LocalServer : public LocalClientServer
//        {
//            Q_OBJECT
//        protected:
//            explicit DA_LocalServer(const QString &identifier,
//                                    const QString &modifier,
//                                    QObject *parent = 0);

//        signals:
//            void notifyNewConnection(QUuid);

//        private slots:
//            void new_connection();

//        private:
//            QLocalServer _server;
//        };
//    }
//}

//#endif // DA_LOCALCLIENTSERVER_H
