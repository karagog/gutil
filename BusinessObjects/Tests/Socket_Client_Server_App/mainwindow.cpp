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
#include <QProcess>
using namespace GUtil;
using namespace BusinessObjects;

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

    if(server)
    {
        _server = new LocalSocketServer(this);
    }
    else
    {
        ui->btnNewClient->setVisible(false);

        _client = new LocalSocketClient(this);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::start_new_client()
{
    QProcess *p = new QProcess(this);
    p->start(_process_name);
}

void MainWindow::send_message()
{
    QString message("Hello");
    if(_server)
    {
        // Broadcast the message
        _server->SendMessage(message);
    }
    if(_client)
        _client->SendMessage(message);
}
