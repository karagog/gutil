#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Core/exception.h"
#include <QLocalSocket>
#include <QLocalServer>

using namespace GUtil;
using namespace DataAccess;

#define SOCKET_ID "gutil_socket_test"

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

        srv = new QLocalServer(this);
        connect(srv, SIGNAL(newConnection()),
                this, SLOT(new_connection()));

        srv->listen(SOCKET_ID);
    }
    else
    {
        ui->lblServer->setText("Client");

        sock = new GSocketIODevice(new QLocalSocket(this));
        connect(sock, SIGNAL(ReadyRead()),
                this, SLOT(read_sock()));

        sock->Socket().connectToServer(SOCKET_ID);
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
        sock = new GSocketIODevice(srv->nextPendingConnection());
        connect(sock, SIGNAL(ReadyRead()),
                this, SLOT(read_sock()));
    }
}
