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

#include "DataAccess/gtcpsocketiodevice.h"
#include <QMainWindow>

class QTcpServer;

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

    GUtil::GTcpSocketIODevice *sock;
    QTcpServer *srv;

};

#endif // MAINWINDOW_H
