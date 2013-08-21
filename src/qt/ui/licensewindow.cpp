/*Copyright 2012 George Karagoulis
  
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
    
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "licensewindow.h"
#include "ui_licensewindow.h"
#include "gutil_macros.h"

NAMESPACE_GUTIL2(QT, UI);


LicenseWindow::LicenseWindow(const QString &license_text, QWidget *parent)
    :QDialog(parent),
      ui(new Ui::LicenseWindow)
{
    ui->setupUi(this);

    SetLicenseText(license_text);
}

LicenseWindow::~LicenseWindow()
{
    delete ui;
}


void LicenseWindow::SetLicenseText(const QString &txt)
{
    ui->txt_license->setText(txt);
}

QString LicenseWindow::GetLicenseText() const
{
    return ui->txt_license->toPlainText();
}


END_NAMESPACE_GUTIL2;
