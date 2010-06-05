/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "line_edit_with_button.h"
#include "effectswidgets.h"
#include "ui_line_edit_with_button.h"
#include <QShowEvent>
#include <QEvent>
using namespace GUtil::QtControls;
using namespace GUtil::QtControls::EffectsWidgets;

line_edit_with_button::line_edit_with_button(QWidget *par, bool btn_visible, int fade_duration) :
    QWidget(par),
    ui(new Ui::line_edit_with_button)
{
    ui->setupUi(this);

    ui->pushButton->setVisible(false);

    // Attach a fader to the button
    _fader = new FaderWidget(ui->pushButton,
                             fade_duration == -1 ? 500 : fade_duration,
                             350);

    if(btn_visible)
        _fader->fadeIn();

    // Set up the focus policies so that my child widgets NEVER get focus
    ui->lineEdit->setFocusPolicy(Qt::NoFocus);
    ui->pushButton->setFocusPolicy(Qt::NoFocus);
    setFocusPolicy(Qt::StrongFocus);
}

line_edit_with_button::~line_edit_with_button()
{
    delete ui;
}

void line_edit_with_button::showButton()
{
    if(ui->pushButton->isHidden())
        _fader->fadeIn();
}

void line_edit_with_button::hideButton()
{
    if(!ui->pushButton->isHidden())
        _fader->fadeIn();
}

void line_edit_with_button::toggleButton()
{
    _fader->toggleFade();
}

void line_edit_with_button::focusInEvent(QFocusEvent *e)
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

FaderWidget * line_edit_with_button::faderWidget() const
{
    return _fader;
}
