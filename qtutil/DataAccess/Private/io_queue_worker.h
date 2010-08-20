#ifndef IO_QUEUE_MANAGER_H
#define IO_QUEUE_MANAGER_H

#include <QObject>
#include <QUuid>
#include <QMutex>
#include <QByteArray>
#include <QQueue>

class QIODevice;

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            // This class manages an io queue, and supports anything that derives from QIODevice, such as a
            //   QLocalSocket or QFile or QTCPSocket

            class IO_Queue_Worker : public QObject
            {
                Q_OBJECT
            public:
                QUuid uniqueId() const;

                IO_Queue_Worker(QIODevice *, QObject *parent = 0);
                ~IO_Queue_Worker();

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

                QMutex io_mutex;
                QMutex in_queue_mutex;
                QMutex out_queue_mutex;

                QIODevice *_io_device;

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
        }
    }
}

#endif // IO_QUEUE_MANAGER_H
