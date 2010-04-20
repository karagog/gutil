#include "line_edit_with_button.h"
#include "ui_line_edit_with_button.h"
#include <QShowEvent>

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

void line_edit_with_button::setButtonToolTip(const QString&s)
{
    ui->pushButton->setToolTip(s);
}

void line_edit_with_button::setLineEditToolTip(const QString &s)
{
    ui->lineEdit->setToolTip(s);
}

QString line_edit_with_button::text() const
{
    return ui->lineEdit->text();
}

void line_edit_with_button::setText(const QString &t)
{
    ui->lineEdit->setText(t);
}

void line_edit_with_button::selectAll() const
{
    ui->lineEdit->selectAll();
}

void line_edit_with_button::_btn_clicked()
{
    emit clicked();
}

void line_edit_with_button::_txt_changed(QString val)
{
    emit textChanged(val);
}

void line_edit_with_button::focusInEvent(QFocusEvent *ev)
{
    QWidget::focusInEvent(ev);

    ui->lineEdit->setFocus();
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
