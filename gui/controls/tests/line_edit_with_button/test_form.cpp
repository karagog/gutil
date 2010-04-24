#include "test_form.h"
#include "ui_test_form.h"
#include <QLineEdit>
#include "line_edit_with_button.h"
#include "effectswidgets.h"
using namespace GUtil::QtControls;

test_form::test_form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::test_form)
{
    ui->setupUi(this);


    line_edit_with_button *le = new line_edit_with_button(this, false);
    ui->verticalLayout->addWidget(le);

    connect(ui->lineEditWidget->pushButton(), SIGNAL(clicked()), this, SLOT(process_click()));

    connect(ui->pushButton_2, SIGNAL(clicked()), le, SLOT(toggleButton()));

    le->faderWidget()->setFadeDuration(2000);
}

test_form::~test_form()
{
    delete ui;
}

void test_form::process_click()
{
    ui->label->setText(ui->lineEditWidget->lineEdit()->text());
}

void test_form::changeEvent(QEvent *e)
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
