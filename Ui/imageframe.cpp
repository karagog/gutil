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

#include "imageframe.h"
#include "gutil_macros.h"
GUTIL_USING_NAMESPACE(UI);


ImageFrame::ImageFrame(QWidget *parent)
    :QWidget(parent)
{}

ImageFrame::ImageFrame(const QString &path, QWidget *parent)
    :QWidget(parent)
{
    SetImagePath(path);
}

void ImageFrame::SetImagePath(const QString &p)
{
    m_imagePath = p;

    setStyleSheet(QString("image: url(%1)")
                  .arg(p));
    show();
}
