#include "line_edit_with_button.h"
#include "ui_line_edit_with_button.h"

line_edit_with_button::line_edit_with_button(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::line_edit_with_button)
{
    ui->setupUi(this);
}

line_edit_with_button::~line_edit_with_button()
{
    delete ui;
}

QString line_edit_with_button::text()
{
    return ui->lineEdit->text();
}

void line_edit_with_button::_btn_clicked()
{
    emit clicked();
}

void line_edit_with_button::_txt_changed(QString val)
{
    emit textChanged(val);
}

void line_edit_with_button::changeEvent(QEvent *e)
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
