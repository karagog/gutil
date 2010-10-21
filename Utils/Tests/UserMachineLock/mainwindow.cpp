#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Core/exception.h"
#include "Logging/globallogger.h"
#include <QMessageBox>
#include <QFileDialog>
using namespace GUtil::Utils;
using namespace GUtil::Logging;
using namespace GUtil;

#define DEFAULT_ID "UserMachineLockTest"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _mutex = new UserMachineMutex();
    _rw_lock = new UserMachineReadWriteLock();

    combo_box_index_changed(ui->cmb_lockSelect->currentIndex());

    connect(&ui->line_edit->pushButton(), SIGNAL(clicked()), this, SLOT(get_file()));
    connect(&ui->line_edit->lineEdit(), SIGNAL(textChanged(QString)), this, SLOT(set_lock_file(QString)));

    set_lock_file();

    ui->cmb_lockSelect->setCurrentIndex(1);
}

void MainWindow::get_file()
{
    QString file = QFileDialog::getOpenFileName(this, "Select a file to lock");

    if(!file.isEmpty())
        ui->line_edit->lineEdit().setText(file);
}

void MainWindow::set_lock_file(const QString &new_filename)
{
    unlock();

    if(new_filename.isEmpty())
    {
        ui->lbl_status->setText("No lockfile set, using default identifier");

        _mutex->SetUserMachineLockIdentifier(DEFAULT_ID, "");
        _rw_lock->SetUserMachineLockIdentifier(DEFAULT_ID, "");
    }
    else
    {
        _mutex->SetUserMachineLockFileName(new_filename);
        _rw_lock->SetUserMachineLockFileName(new_filename);
    }

    ui->lbl_filename->setText(_mutex->FileNameForMachineLock());
}

void MainWindow::lockWrite()
{
    lock(false);
}

void MainWindow::lockRead()
{
    lock(true);
}

void MainWindow::lock(bool for_read)
{
    try
    {
        if(using_mutex())
            _mutex->LockMutexOnMachine();
        else
        {
            if(for_read)
                _rw_lock->LockForReadOnMachine();
            else
                _rw_lock->LockForWriteOnMachine();
        }
    }
    catch(Core::LockException &ex)
    {
        GlobalLogger::LogException(ex);
        return;
    }
    catch(Core::Exception &ex)
    {
        GlobalLogger::LogException(ex);
        return;
    }

    ui->lbl_status->setText(QString("Locked") + (using_mutex() ?
                            "" : QString(" for ") + (for_read ?
                                   "Read" : "Write")));
}

void MainWindow::unlock()
{
    _mutex->UnlockForMachine();
    _rw_lock->UnlockForMachine();

    ui->lbl_status->clear();
}

void MainWindow::combo_box_index_changed(int ind)
{
    set_mutex_lock(using_mutex());
}

void MainWindow::set_mutex_lock(bool mutex)
{
    ui->btn_lockRead->setVisible(!mutex);
    ui->btn_lockWrite->setText(mutex ? "Lock" : "Lock Write");
}

bool MainWindow::using_mutex()
{
    return ui->cmb_lockSelect->currentIndex() == 0;
}

Utils::MachineLockBase *MainWindow::_get_lock()
{
    if(using_mutex())
        return _mutex;
    else
        return _rw_lock;
}

MainWindow::~MainWindow()
{
    delete ui;

    delete _mutex;
    delete _rw_lock;
}
