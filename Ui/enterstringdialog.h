/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef ENTERSTRINGDIALOG_H
#define ENTERSTRINGDIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace GUtil{ namespace UI{


/** A ready-made dialog to prompt the user for a string value. */
class EnterStringDialog :
        public QDialog
{
    Q_OBJECT
public:

    explicit EnterStringDialog(const QString &title, const QString &prompt = QString::null, QWidget *parent = 0);

    /** Provide public access to our line edit, to allow external customization without
        having to derive from this class.
    */
    QLineEdit LineEdit;

    /** Access to the string the user entered.  This is only valid if the dialog was accepted. */
    QString String;


protected:

    void accept();

};


}}

#endif // ENTERSTRINGDIALOG_H
