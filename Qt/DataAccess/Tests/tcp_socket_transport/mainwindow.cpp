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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Core/exception.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <QProcess>

using namespace GUtil;
using namespace DataAccess;

MainWindow::MainWindow(bool server, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    sock(0),
    srv(0)
{
    ui->setupUi(this);

    if(server)
    {
        ui->lblServer->setText("Server");

        srv = new QTcpServer(this);
        connect(srv, SIGNAL(newConnection()),
                this, SLOT(new_connection()));

        srv->listen(QHostAddress("127.0.0.1"));

        // Start a duplicate process, that will connect to the server
        QProcess *p = new QProcess(qApp);
        p->start(qApp->argv()[0], QStringList(QString("%1").arg(srv->serverPort())));
    }
    else
    {
        ui->lblServer->setText("Client");

        sock = new GTcpSocketIODevice(new QTcpSocket(this));
        connect(sock, SIGNAL(ReadyRead()), this, SLOT(read_sock()));

        sock->Socket().connectToHost("127.0.0.1", QString(qApp->argv()[1]).toUInt());
        sock->Socket().waitForConnected(5000);

        if(!sock->IsConnected())
            THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                      "Not Connected");
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    if(sock)
        delete sock;

    if(srv)
        delete srv;
}

void MainWindow::read_sock()
{
    ui->lblReceived->setText(sock->ReceiveData());
}

void MainWindow::send_data()
{
    sock->Write(ui->txtSend->toPlainText().toAscii());
}

void MainWindow::new_connection()
{
    if(srv->hasPendingConnections())
    {
        sock = new GTcpSocketIODevice(srv->nextPendingConnection());
        connect(sock, SIGNAL(ReadyRead()), this, SLOT(read_sock()));
    }
}
