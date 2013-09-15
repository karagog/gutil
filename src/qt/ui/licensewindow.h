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

#ifndef LICENSEWINDOW_H
#define LICENSEWINDOW_H

#include <QDialog>

namespace Ui {
class LicenseWindow;
}


namespace GUtil{ namespace QT{ namespace UI{


class LicenseWindow : public QDialog
{
    Q_OBJECT
    Ui::LicenseWindow *ui;
public:

    explicit LicenseWindow(const QString &license_text, QWidget *parent = 0);
    ~LicenseWindow();

    /** Sets the window's license text. */
    void SetLicenseText(const QString &txt);

    /** Returns the current license text. */
    QString GetLicenseText() const;

};


}}}

#endif // LICENSEWINDOW_H
