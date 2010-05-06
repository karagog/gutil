#include "flattreemodeltest.h"
#include "myflattreeview.h"
#include "ui_flattreemodeltest.h"
#include <QFileSystemModel>
using namespace GUtil::QtUtil;

flatTreeModelTest::flatTreeModelTest(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::flatTreeModelTest)
{
    ui->setupUi(this);

    QFileSystemModel *dm = new QFileSystemModel(this);
    dm->setRootPath("C:\\");

    myFlatTreeModel *ftv = new myFlatTreeModel(0);
    ftv->setSourceModel(dm);

    ui->treeView->setModel(dm);
    ui->listView->setModel(ftv);

    connect(ui->pushButton, SIGNAL(clicked()), ftv, SLOT(refreshSourceModel()));
}

flatTreeModelTest::~flatTreeModelTest()
{
    delete ui;
}

void flatTreeModelTest::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
