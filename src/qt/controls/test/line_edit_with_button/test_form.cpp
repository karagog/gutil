#include "test_form.h"
#include "ui_test_form.h"
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include "Controls/line_edit_with_button.h"
#include "Controls/faderwidget.h"
using namespace GUtil::Controls;

test_form::test_form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::test_form)
{
    ui->setupUi(this);

    setTabOrder(ui->lineEditWidget, ui->pushButton_2);

    line_edit_with_button *le = new line_edit_with_button(this, false);
    ui->verticalLayout->addWidget(le);

    connect(&ui->lineEditWidget->pushButton(), SIGNAL(clicked()), this, SLOT(process_click()));

    connect(ui->pushButton_2, SIGNAL(clicked()), le, SLOT(toggleButton()));

    connect(&ui->widget->PushButton(), SIGNAL(clicked()), this, SLOT(progress_bar_clicked()));

    ui->widget->setButtonEnabled(true);
    ui->widget->Label().setText("Hello World");

    le->faderWidget()->setFadeDuration(2000);
}

test_form::~test_form()
{
    delete ui;
}

void test_form::process_click()
{
    ui->label->setText(ui->lineEditWidget->lineEdit().text());
}

void test_form::progress_bar_clicked()
{
    ui->widget->ProgressBar().setValue(ui->widget->ProgressBar().value() + 5);
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
