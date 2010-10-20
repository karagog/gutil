#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Utils/usermachinemutex.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow,
                    public GUtil::Utils::UserMachineMutex
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void lock();
    void unlock();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
