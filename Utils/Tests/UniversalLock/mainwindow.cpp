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

#define DEFAULT_FILE_PATH "temp_file"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _universal_lock(DEFAULT_FILE_PATH)
{
    ui->setupUi(this);

    ui->file_chooser->SetSelectedFile(DEFAULT_FILE_PATH);

    connect(&ui->file_chooser->lineEdit(), SIGNAL(textChanged(QString)),
            this, SLOT(file_selected()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::file_selected()
{
    _universal_lock.SetFilePath(ui->file_chooser->GetSelectedFile());
    ui->label->clear();
}

void MainWindow::lock()
{
    _universal_lock.Lock(true);
    ui->label->setText("File Locked");
}

void MainWindow::unlock()
{
    _universal_lock.Unlock();
    ui->label->clear();
}
