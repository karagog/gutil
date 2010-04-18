#include "test_form.h"
#include "ui_test_form.h"

test_form::test_form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::test_form)
{
    ui->setupUi(this);

    connect(ui->lineEditWidget, SIGNAL(clicked()), this, SLOT(process_click()));
}

test_form::~test_form()
{
    delete ui;
}

void test_form::process_click()
{
    ui->label->setText(ui->lineEditWidget->text());
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
