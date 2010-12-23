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
#include <QProcess>
#include <QList>
using namespace GUtil;
using namespace BusinessObjects;

#define IDENTIFIER "socket_test_app"
#define MODIFIER "main"

MainWindow::MainWindow(bool server,
                       const QString &proc_name,
                       QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _process_name(proc_name),
    _client(0),
    _server(0)
{
    ui->setupUi(this);

    ui->lblServer->setText(server ? "Server" : "Client");

    try
    {
        if(server)
        {
            _server = new LocalSocketServer(this);

            connect(_server, SIGNAL(NewConnection(QUuid)),
                    this, SLOT(new_connection(QUuid)));
            connect(_server, SIGNAL(Disconnected(QUuid)),
                    this, SLOT(client_disconnected(QUuid)));

            _server->ListenForConnections(IDENTIFIER, MODIFIER);
        }
        else
        {
            ui->btnNewClient->setVisible(false);
            ui->lstClients->setVisible(false);

            _client = new LocalSocketClient(this);

            _client->ConnectToServer(IDENTIFIER, MODIFIER);
        }
    }
    catch(Core::Exception &ex)
    {
        ui->lblServer->setText(QString("FAILED TO INITIALIZE: %1")
                               .arg(ex.GetMessage().c_str()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;

    if(_client)
    {
        _client->Disconnect();
        delete _client;
    }

    if(_server)
    {
        _server->ShutDownServer();
        delete _server;
    }
}

void MainWindow::start_new_client()
{
    QProcess *p = new QProcess(this);
    p->start(_process_name, QStringList("client"));
}

void MainWindow::new_connection(const QUuid &id)
{
    ui->lstClients->addItem(id.toString());
}

void MainWindow::client_disconnected(const QUuid &id)
{
    QList<QListWidgetItem *> l(
            ui->lstClients->findItems(id.toString(), Qt::MatchContains));
    int cnt(l.count());
    if(cnt > 0)
    {
        int row(ui->lstClients->row(l[0]));
        if(row >= 0)
            ui->lstClients->takeItem(row);
    }
}

void MainWindow::send_message()
{
    QByteArray message("Hello");
    if(_server)
    {
        QList<QListWidgetItem *> l(ui->lstClients->selectedItems());
        if(l.count() == 0)
        {
            // Broadcast the message
            _server->SendMessage(message);
        }
        else
        {
            foreach(QListWidgetItem *item, l)
                _server->SendMessage(message, item->text());
        }
    }
    if(_client)
        _client->SendMessage(message);
}
