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

#include "filechooser.h"
#include <QFileDialog>
using namespace GUtil::Controls;

FileChooser::FileChooser(QWidget *parent)
    :line_edit_with_button(parent),
    _p_SelectionType( ChooseOpenFile )
{
    connect(&pushButton(), SIGNAL(clicked()),
            this, SLOT(_button_clicked()));
}

void FileChooser::_button_clicked()
{
    QString *fn(0);
    QStringList *lst_fn(0);

    switch(GetSelectionType())
    {
    case ChooseOpenFile:
        fn = new QString(QFileDialog::getOpenFileName(this,
                                                      GetDialogCaption(),
                                                      GetStartingDirectory(),
                                                      GetSelectionFilters()));
        break;
    case ChooseOpenFiles:
        lst_fn = new QStringList(QFileDialog::getOpenFileNames(this,
                                                               GetDialogCaption(),
                                                               GetStartingDirectory(),
                                                               GetSelectionFilters()));
        break;
    case ChooseNewFile:
        fn = new QString(QFileDialog::getSaveFileName(this,
                                                      GetDialogCaption(),
                                                      GetStartingDirectory(),
                                                      GetSelectionFilters()));
        break;
    case ChooseExistingDirectory:
        fn = new QString(QFileDialog::getExistingDirectory(this,
                                                           GetDialogCaption(),
                                                           GetStartingDirectory()));
        break;
    default:
        return;
    }


    if(fn)
    {
        if(fn->length() > 0)
            SetSelectedFile(*fn);

        delete fn;
    }

    if(lst_fn)
    {
        delete lst_fn;
    }
}

void FileChooser::SetSelectedFile(const QString &s)
{
    _selected_file = s;
    lineEdit().setText(s);
}

QString FileChooser::GetSelectedFile() const
{
    return _selected_file;
}