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

GUTIL_USING_NAMESPACE(Custom);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    _data(4, QStringList() <<
                           "Hi" <<
                           "Second" <<
                           "More Data" <<
                           "HELLO WORLD")
{
    ui->setupUi(this);

    _data.AddNewRow(GVariantList() << 5 << "Data" << 1231 << "Howdy!");
    _data.AddNewRow(GVariantList() << 7 << "Dat" << 11 << "Sexy");

    _model.SetDataTable(&_data);

    ui->tableView->setModel(&_model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::insert_row()
{
    QModelIndexList lst(ui->tableView->selectionModel()->selectedRows());
    if(lst.count() > 0)
        _model.insertRows(lst.first().row(), lst.count());
}

void MainWindow::remove_row()
{
    QModelIndexList lst(ui->tableView->selectionModel()->selectedRows());
    if(lst.count() > 0)
        _model.removeRows(lst.first().row(), lst.count());
}

void MainWindow::insert_column()
{
    QModelIndexList lst(ui->tableView->selectionModel()->selectedColumns());
    if(lst.count() > 0)
        _model.insertColumns(lst.first().column(), lst.count());
}

void MainWindow::remove_column()
{
    QModelIndexList lst(ui->tableView->selectionModel()->selectedColumns());
    if(lst.count() > 0)
        _model.removeColumns(lst.first().column(), lst.count());
}
