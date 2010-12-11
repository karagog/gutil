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

#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QFile>

namespace Ui {
    class MainWindow;
}

class MainWindow :
        public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    inline QString FileName() const{ return _filename; }

private slots:
    void update_file();
    void caught_file_updated();

    void choose_file();

    void open_read();
    void open_readwrite();
    void open_write();
    void close_file();

    void write_bytes();

private:
    Ui::MainWindow *ui;

    QFileSystemWatcher *fsw;
    QString _filename;

    QFile _file;

    int update_count;
};

#endif // MAINWINDOW_H
