#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "DataAccess/gsocketiodevice.h"
#include <QMainWindow>

class QLocalServer;

namespace Ui {
    class MainWindow;
}

class MainWindow :
        public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool server, QWidget *parent = 0);
    ~MainWindow();


private slots:

    void read_sock();
    void new_connection();

    void send_data();


private:
    Ui::MainWindow *ui;

    GUtil::DataAccess::GSocketIODevice *sock;
    QLocalServer *srv;

};

#endif // MAINWINDOW_H
