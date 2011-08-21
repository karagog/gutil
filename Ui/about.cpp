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

#include "about.h"
#include "gutil_macros.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>

GUTIL_USING_NAMESPACE(UI);

#define TITLE_HEIGHT 40

#define PUSH_BUTTON_WIDTH 100

About::About(QWidget *parent)
    :QDialog(parent)
{
    // Prepare the dialog layout
    resize(400, 300);

    Title.setAlignment(Qt::AlignHCenter);
    BuildInfo.setAlignment(Qt::AlignHCenter);
    {
        QFont f;
        f.setBold(true);
        f.setPixelSize(20);
        Title.setFont(f);
        Title.setFixedHeight(TITLE_HEIGHT);
    }

    QHBoxLayout *top_level_layout(new QHBoxLayout);
    _imageFrame.hide();
    top_level_layout->addWidget(&_imageFrame);
    top_level_layout->setAlignment(&_imageFrame, Qt::AlignTop);

    QVBoxLayout *vbl( new QVBoxLayout );
    top_level_layout->addLayout(vbl);
    vbl->addWidget(&Title);
    vbl->addWidget(&BuildInfo);
    vbl->addWidget(&Text);
    {
        // Set up the buttons at the bottom of the widget
        m_buttonWidget = new QWidget(this);
        QHBoxLayout *hbl( new QHBoxLayout );
        hbl->setContentsMargins(0,0,0,0);
        QPushButton *aboutQt( new QPushButton("About Qt", this) );
        aboutQt->setMinimumWidth(PUSH_BUTTON_WIDTH);
        QPushButton *ok( new QPushButton("Ok", this) );

        hbl->addWidget(aboutQt);
        hbl->addStretch(1);
        hbl->addWidget(ok);
        m_buttonWidget->setLayout(hbl);

        connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
        connect(aboutQt, SIGNAL(clicked()), qApp, SLOT(aboutQt()));

        vbl->addWidget(m_buttonWidget);

        ok->setFocus();

        m_buttonList.append(aboutQt);
        m_buttonList.append(ok);
    }
    setLayout(top_level_layout);

    Text.setReadOnly(true);
}

void About::SetImage(const QString &filename)
{
    if(filename.isNull())
    {
        _imageFrame.hide();
        setWindowIcon(QIcon());
    }
    else
    {
        _imageFrame.SetImagePath(filename);
        _imageFrame.setFixedSize(TITLE_HEIGHT, TITLE_HEIGHT);
        setWindowIcon(QIcon(filename));
    }
}

void About::AddPushButton(QPushButton *pb)
{
    delete m_buttonWidget->layout();
    QHBoxLayout *hbl(new QHBoxLayout(m_buttonWidget));

    // Insert the new button before the ok
    m_buttonList.insert(m_buttonList.count() - 1, pb);
    pb->setMinimumWidth(PUSH_BUTTON_WIDTH);

    for(int i(0); i < m_buttonList.count(); i++)
    {
        hbl->addWidget(m_buttonList[i]);

        if(i < (m_buttonList.count() - 1))
            hbl->addStretch(1);
    }
}
