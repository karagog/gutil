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
#include <QFile>

#define FILENAME "filewatcher.testfile"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    update_count = 0;

    update_file();

    fsw = new QFileSystemWatcher(QStringList(FILENAME));
    connect(fsw, SIGNAL(fileChanged(QString)), this, SLOT(caught_file_updated()));
}

void MainWindow::update_file()
{
    QFile f(FILENAME);
    if(!f.open(QFile::WriteOnly))
    {
        ui->lbl_notify->setText(QString("Couldn't open file: %1").arg(FILENAME));
    }
    f.close();
}

void MainWindow::caught_file_updated()
{
    ui->lbl_notify->setText(QString("Caught update #%1").arg(++update_count));
}

MainWindow::~MainWindow()
{
    delete ui;
}
