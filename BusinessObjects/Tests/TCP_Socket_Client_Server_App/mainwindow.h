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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "BusinessObjects/tcpsocketclient.h"
#include "BusinessObjects/tcpsocketserver.h"
#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool server, QWidget *parent = 0);
    ~MainWindow();


private slots:

    void start_new_client();
    void send_message();

    void receive_message(const QUuid &id = QUuid());

    void new_connection(const QUuid &);
    void client_disconnected(const QUuid &);


private:
    Ui::MainWindow *ui;

    GUtil::BusinessObjects::TcpSocketClient *_client;
    GUtil::BusinessObjects::TcpSocketServer *_server;

};

#endif // MAINWINDOW_H
