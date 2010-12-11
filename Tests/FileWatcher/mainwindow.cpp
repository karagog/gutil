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
#include "Controls/line_edit_with_button.h"
#include <QFile>
#include <QFileDialog>

#define DEFAULT_FILENAME "filewatcher.testfile"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _filename(DEFAULT_FILENAME),
    _file(DEFAULT_FILENAME)
{
    ui->setupUi(this);
    ui->txt_file_chooser->lineEdit().setReadOnly(true);

    ui->btnWriteBytes->setEnabled(false);

    update_count = 0;

    update_file();

    fsw = new QFileSystemWatcher(QStringList(_filename));
    connect(fsw, SIGNAL(fileChanged(QString)), this, SLOT(caught_file_updated()));

    connect(&ui->txt_file_chooser->pushButton(), SIGNAL(clicked()),
            this, SLOT(choose_file()));
}

void MainWindow::update_file()
{
    QFile f(FileName());
    if(!f.open(QFile::WriteOnly))
    {
        ui->lbl_notify->setText(QString("Couldn't open file: %1").arg(FileName()));
    }
    f.close();
}

void MainWindow::caught_file_updated()
{
    ui->lbl_notify->setText(QString("Caught update #%1").arg(++update_count));
}

void MainWindow::choose_file()
{
    QString f = QFileDialog::getOpenFileName(this, "File to Watch");
    if(f.length() == 0)
        return;

    _filename = f;
    _file.setFileName(f);

    fsw->removePath(FileName());
    fsw->addPath(f);
    ui->txt_file_chooser->lineEdit().setText(FileName());
    update_count = -1;
    caught_file_updated();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_read()
{
    _file.open(QFile::ReadOnly);
}

void MainWindow::open_readwrite()
{
    _file.open(QFile::ReadWrite);

    ui->btnWriteBytes->setEnabled(true);
}

void MainWindow::open_write()
{
    _file.open(QFile::WriteOnly);
    ui->btnWriteBytes->setEnabled(true);
}

void MainWindow::close_file()
{
    _file.close();
    ui->btnWriteBytes->setEnabled(false);
}

void MainWindow::write_bytes()
{
    if(_file.isOpen())
        _file.write("Hello World!");
}
