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

#ifndef FILECHOOSER_H
#define FILECHOOSER_H

#include "line_edit_with_button.h"
#include "gutil_macros.h"

namespace GUtil
{
    namespace Controls
    {
        class FileChooser :
                public line_edit_with_button
        {
            Q_OBJECT
        public:

            FileChooser(QWidget *parent = 0);

            // Open the file dialog and ask for a new file
            inline void GetFileSelection(){ _button_clicked(); }

            void SetSelectedFile(const QString &);
            QString GetSelectedFile() const;


            PROPERTY( DialogCaption, QString );
            PROPERTY( StartingDirectory, QString );
            PROPERTY( SelectionFilters, QString );


            enum SelectionTypeEnum
            {
                ChooseOpenFile,
                ChooseOpenFiles,
                ChooseNewFile,
                ChooseExistingDirectory
            };

            PROPERTY( SelectionType, SelectionTypeEnum );


        private slots:

            void _button_clicked();


        private:

            QString _selected_file;

        };
    }
}

#endif // FILECHOOSER_H
