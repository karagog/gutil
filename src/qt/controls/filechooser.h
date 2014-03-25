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

#ifndef FILECHOOSER_H
#define FILECHOOSER_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include "lineeditwithbutton.h"
#include "gutil_exception.h"
#include <QFileDialog>

namespace GUtil{ namespace QT{


/** A control to facilitate choosing a file.  It is a line edit with a button,
    and when you click the button it lets you use the native file chooser dialogs to
    browse for files.
*/
class FileChooser :
        public LineEditWithButton
{
    Q_OBJECT
public:

    explicit FileChooser(QWidget *parent = 0)
        :LineEditWithButton(parent), _p_SelectionType( ChooseOpenFile )
    { connect(&PushButton(), SIGNAL(clicked()), this, SLOT(_button_clicked())); }

    /** Open the file dialog and ask for a new file */
    void GetFileSelection(){ _button_clicked(); }

    /** Sets the currently selected file to the given filepath */
    void SetSelectedFile(const QString &s){
        _selected_file = s;
        LineEdit().setText(s);
    }

    /** Returns the currently selected file */
    QString GetSelectedFile() const{ return _selected_file; }


    PROPERTY( DialogCaption, QString );
    PROPERTY( StartingDirectory, QString );
    PROPERTY( SelectionFilters, QString );


    /** Describes different methods for selecting files */
    enum SelectionTypeEnum
    {
        /** Selects a file */
        ChooseOpenFile,
        /** Selects multiple files */
        ChooseOpenFiles,
        /** Selects a filename which doesn't exist (strictly a new file, or warns you
            that the file will be overwritten if it exists)
        */
        ChooseNewFile,
        /** Selects a directory*/
        ChooseExistingDirectory
    };

    PROPERTY( SelectionType, SelectionTypeEnum );


private slots:

    void _button_clicked(){
        QString fn;
        QStringList lst_fn;

        switch(GetSelectionType())
        {
        case ChooseOpenFile:
            fn = QFileDialog::getOpenFileName(this,
                                              GetDialogCaption(),
                                              GetStartingDirectory(),
                                              GetSelectionFilters());
            break;
        case ChooseOpenFiles:
            lst_fn = QFileDialog::getOpenFileNames(this,
                                                   GetDialogCaption(),
                                                   GetStartingDirectory(),
                                                   GetSelectionFilters());
            break;
        case ChooseNewFile:
            fn = QFileDialog::getSaveFileName(this,
                                              GetDialogCaption(),
                                              GetStartingDirectory(),
                                              GetSelectionFilters());
            break;
        case ChooseExistingDirectory:
            fn = QFileDialog::getExistingDirectory(this,
                                                   GetDialogCaption(),
                                                   GetStartingDirectory());
            break;
        default:
            THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
        }


        if(!lst_fn.isEmpty())
            SetSelectedFile(lst_fn.join("; "));
        else if(!fn.isEmpty())
            SetSelectedFile(fn);
    }


private:

    QString _selected_file;

};


}}

#endif // GUI_FUNCTIONALITY

#endif // FILECHOOSER_H
