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
#include "aboutgutil.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>

NAMESPACE_GUTIL2(QT, UI);

#define TITLE_HEIGHT 40

#define PUSH_BUTTON_WIDTH 100

About::About(QWidget *parent, bool show_about_gutil)
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
        m_buttonList.append(aboutQt);
        aboutQt->setMinimumWidth(PUSH_BUTTON_WIDTH);
        QPushButton *aboutGUtil( 0 );
        if(show_about_gutil)
        {
            aboutGUtil = new QPushButton("About GUtil", this);
            m_buttonList.append(aboutGUtil);
            aboutGUtil->setMinimumWidth(PUSH_BUTTON_WIDTH);
            connect(aboutGUtil, SIGNAL(clicked()), this, SLOT(_about_gutil()));
        }
        QPushButton *ok( new QPushButton("Ok", this) );
        m_buttonList.append(ok);

        hbl->addWidget(aboutQt);
        hbl->addStretch(1);
        if(aboutGUtil)
        {
            hbl->addWidget(aboutGUtil);
            hbl->addStretch(1);
        }
        hbl->addWidget(ok);
        m_buttonWidget->setLayout(hbl);

        connect(aboutQt, SIGNAL(clicked()), qApp, SLOT(aboutQt()));
        connect(ok, SIGNAL(clicked()), this, SLOT(accept()));

        vbl->addWidget(m_buttonWidget);

        ok->setFocus();
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
        _imageFrame.setFixedSize(TITLE_HEIGHT, TITLE_HEIGHT);
        _imageFrame.setStyleSheet(QString("image: url(%1);").arg(filename));
        _imageFrame.show();
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

void About::_about_gutil()
{
    AboutGUtil(this).exec();
}


END_NAMESPACE_GUTIL2;
