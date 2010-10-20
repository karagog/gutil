#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Core/exception.h"
#include "Logging/globallogger.h"
#include <QMessageBox>
using namespace GUtil::Utils;
using namespace GUtil::Logging;
using namespace GUtil;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    UserMachineMutex("User_machine_lock_test"),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

void MainWindow::lock()
{
    try
    {
        LockForUserOnMachine();
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

    ui->lbl_status->setText("Locked");
}

void MainWindow::unlock()
{
    UnlockForUserOnMachine();
    ui->lbl_status->clear();
}

MainWindow::~MainWindow()
{
    delete ui;
}
