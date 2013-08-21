#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Utils/usermachinelocks.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void lockWrite();
    void lockRead();
    void unlock();

    void get_file();
    void set_lock_file(const QString &fn = "");

    void combo_box_index_changed(int);

private:
    Ui::MainWindow *ui;

    void lock(bool for_read);
    void set_mutex_lock(bool);

    bool using_mutex();

    GUtil::Utils::MachineLockBase *_get_lock();

    GUtil::Utils::UserMachineMutex *_mutex;
    GUtil::Utils::UserMachineReadWriteLock *_rw_lock;
};

#endif // MAINWINDOW_H
