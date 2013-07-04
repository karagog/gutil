/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "gutil_macros.h"
#include "enterstringdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>

NAMESPACE_GUTIL2(QT, UI);

#define DIALOG_WIDTH 400
#define DIALOG_HEIGHT 100

EnterStringDialog::EnterStringDialog(const QString &title, const QString &prompt, QWidget *parent)
    :QDialog(parent)
{
    setWindowTitle(title.isNull() ? "Enter String" : title);
    QVBoxLayout *l( new QVBoxLayout );
    if(!prompt.isNull())
    {
        QWidget *tmp;
        l->addWidget(tmp = new QLabel(QString("%1:").arg(prompt), this));
        QFont f;
        f.setBold(true);
        tmp->setFont(f);
    }
    l->addWidget(&LineEdit);

    QDialogButtonBox *bb;
    l->addWidget(bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                           Qt::Horizontal,
                                           this));
    connect(bb, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bb, SIGNAL(rejected()), this, SLOT(reject()));

    setLayout(l);

    resize(DIALOG_WIDTH, DIALOG_HEIGHT);
}

void EnterStringDialog::accept()
{
    String = LineEdit.text();
    QDialog::accept();
}


END_NAMESPACE_GUTIL2;
