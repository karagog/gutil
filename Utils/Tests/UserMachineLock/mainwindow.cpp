#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Core/exception.h"
#include <QMessageBox>
using namespace GUtil::Utils;
using namespace GUtil;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    UserMachineLock("User_machine_lock_test"),
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
        QMessageBox::warning(this, "Lock Failed",
                             QString::fromStdString(ex.Message()));
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
