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
GUTIL_USING_NAMESPACE(BusinessObjects);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cfile("Test_App")
{
    ui->setupUi(this);

    connect(&cfile, SIGNAL(NotifyConfigurationUpdate()),
            this, SLOT(config_updated()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::submitChanges()
{
    bool checked(ui->checkBox->isChecked());
    cfile.SetValue("chk", checked);
}

void MainWindow::config_updated()
{
    bool checked(cfile.Value("chk").toBool());
//    qDebug(QString("These values: %1")
//           .arg(cfile.Values().keys().first()).toStdString().c_str());
    ui->checkBox->setChecked(checked);
}
