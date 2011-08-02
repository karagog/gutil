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

#ifndef IMAGEFRAME_H
#define IMAGEFRAME_H

#include <QWidget>

namespace GUtil{ namespace UI{


// This class simply displays an image from a resource or file path
class ImageFrame :
        public QWidget
{
    Q_OBJECT
public:

    explicit ImageFrame(QWidget *parent = 0);
    explicit ImageFrame(const QString &path, QWidget *parent = 0);

    inline QString ImagePath() const{ return m_imagePath; }


public slots:

    void SetImagePath(const QString &);


private:

    QString m_imagePath;

};


}}
#endif // IMAGEFRAME_H
