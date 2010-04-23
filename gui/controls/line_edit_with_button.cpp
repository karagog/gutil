#include "line_edit_with_button.h"
#include "effectswidgets.h"
#include "ui_line_edit_with_button.h"
#include <QShowEvent>
#include <QEvent>
using namespace GUtil::QtControls;
using namespace GUtil::QtControls::EffectsWidgets;

line_edit_with_button::line_edit_with_button(QWidget *parent, bool btn_visible) :
    QWidget(parent),
    ui(new Ui::line_edit_with_button)
{
    ui->setupUi(this);

    // Attach a fader to the button
    fader = new FaderWidget(ui->pushButton, true, 500, 350);

    if(btn_visible)
        fader->startFading();

    // I want to intercept focus events so I can bring it back to myself
    ui->lineEdit->installEventFilter(this);
    ui->pushButton->installEventFilter(this);
}

line_edit_with_button::~line_edit_with_button()
{
    delete ui;
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

void line_edit_with_button::toggleButton()
{
    fader->toggleFade();
}

void line_edit_with_button::focusInEvent(QFocusEvent *e)
{
    ui->lineEdit->event(e);

    setFocus();
}

void line_edit_with_button::focusOutEvent(QFocusEvent *e)
{
    ui->lineEdit->event(e);
}

void line_edit_with_button::keyPressEvent(QKeyEvent *e)
{
    ui->lineEdit->event(e);
}

void line_edit_with_button::keyReleaseEvent(QKeyEvent *e)
{
    ui->lineEdit->event(e);
}

bool line_edit_with_button::eventFilter(QObject *o, QEvent *ev)
{
    if(o == 0)
        return false;

    switch(ev->type())
    {
    case QEvent::FocusIn:
        // Always bring the focus back to the top widget
        setFocus();
        return true;
    default:
        break;
    }

    return false;
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

QPushButton *line_edit_with_button::pushButton() const
{
    return ui->pushButton;
}

QLineEdit *line_edit_with_button::lineEdit() const
{
    return ui->lineEdit;
}
