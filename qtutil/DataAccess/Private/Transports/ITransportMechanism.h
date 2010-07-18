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

#ifndef ITRANSPORTMECHANISM_H
#define ITRANSPORTMECHANISM_H

#include <QByteArray>
#include <QMutex>
#include <QObject>

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            namespace Transports
            {
                // This is an abstract base class to represent different methods of data transport
                class ITransportMechanism : public QObject
                {
                    Q_OBJECT
                public:

                    // These functions are all atomic with respect to each other;
                    //  feel free to use this class in multiple threads
                    QByteArray receiveData();

                    // Is there data ready to read?
                    bool hasData() const;


                public slots:

                    void sendData(const QByteArray&);


                signals:

                    // Tell the world about the new data that arrived asynchronously
                    void notifyNewData(const QByteArray &);




                protected slots:

                    // Connect this slot to any method of asynchronous update;
                    //   (filesystem watcher update signal, a socket 'readyRead' signal, etc...)
                    virtual void receive_asynchronous_update();


                protected:

                    explicit ITransportMechanism(QObject *parent = 0);

                    // Derived classes must implement these functions
                    //  Note that locking is taken care of by this interface class,
                    //  so you can trust that these are atomic WRT each other
                    virtual void send_data(const QByteArray&) = 0;
                    virtual QByteArray receive_data() = 0;

                    // You must derive from this function to program special logic
                    //   to determine if there's data available.  This will depend
                    //   on the method of transport you're using.
                    virtual void update_has_data_available() = 0;

                    // Only go through this function to update '_has_data' (takes care of locking)
                    void setHasData(bool);


                private:

                    // Protects us so we can be re-entrant when using the transport
                    QMutex _lock;

                    bool _has_data;

                    void receive_data_private(bool asynchronous);

                };
            }
        }
    }
}

#endif // ITRANSPORTMECHANISM_H
