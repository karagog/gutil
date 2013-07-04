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

#ifndef GUTIL_NO_GUI_FUNCTIONALITY


#include "gutil_about.h"
#include "gutil_macros.h"
#include "gutil_application.h"
#include "gutil_licensewindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QtPlugin>

NAMESPACE_GUTIL2(QT, UI);

#define TITLE_HEIGHT 40

#define PUSH_BUTTON_WIDTH 100

void AboutLogic::ShowAbout()
{
    // There is no default implementation
    THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
}

void AboutLogic::ShowAboutQt()
{
    if(qApp) qApp->aboutQt();
}

void AboutLogic::ShowAboutGUtil(QWidget *parent)
{
    // Have to load the about plugin
    QPluginLoader pl("GUtilAboutPlugin" GUTIL_SHAREDLIBRARY_SUFFIX);
    QString error_msg;
    if(pl.load()){
        GUtil::QT::Plugins::IAboutGUtil *about =
                qobject_cast<GUtil::QT::Plugins::IAboutGUtil *>(pl.instance());
        if(about)
            about->ShowAboutGUtil(parent);
        else
            error_msg = "Unable to cast plugin as expected type";
        pl.unload();
    }
    else{
        error_msg = QString("Unable to load about plugin: %1\n\n"
                            "Make sure it is located in the working directory in which the application is executing")
                    .arg(pl.fileName());
    }

    if(!error_msg.isEmpty())
        QMessageBox::critical(0, "ERROR", error_msg, QMessageBox::Ok);
}

void AboutLogic::ShowLicense(QWidget *parent)
{
    LicenseWindow( get_license_text(), parent ).exec();
}

QString AboutLogic::get_license_text()
{
    return QString();
}



About::About(QWidget *parent, bool show_about_gutil_button, bool show_license_button)
    :AboutLogic(parent),
      _dialog(parent),
      m_imageFrame(new QWidget(&_dialog)),
      m_buttonWidget(new QWidget(&_dialog))
{
    // Prepare the dialog layout
    _dialog.resize(400, 300);

    _title.setAlignment(Qt::AlignHCenter);
    _buildinfo.setAlignment(Qt::AlignHCenter);
    {
        QFont f;
        f.setBold(true);
        f.setPixelSize(20);
        _title.setFont(f);
        _title.setFixedHeight(TITLE_HEIGHT);
    }

    QHBoxLayout *top_level_layout(new QHBoxLayout);
    m_imageFrame->hide();
    top_level_layout->addWidget(m_imageFrame);
    top_level_layout->setAlignment(m_imageFrame, Qt::AlignTop);

    QVBoxLayout *vbl( new QVBoxLayout );
    top_level_layout->addLayout(vbl);
    vbl->addWidget(&_title);
    vbl->addWidget(&_buildinfo);
    vbl->addWidget(&_text);
    {
        // Set up the buttons at the bottom of the widget
        m_buttonWidget = new QWidget(&_dialog);
        QHBoxLayout *hbl( new QHBoxLayout );
        hbl->setContentsMargins(0,0,0,0);
        QPushButton *aboutQt( new QPushButton("About Qt", &_dialog) );
        m_buttonList.append(aboutQt);
        aboutQt->setMinimumWidth(PUSH_BUTTON_WIDTH);
        QPushButton *aboutGUtil( 0 );
        if(show_about_gutil_button)
        {
            aboutGUtil = new QPushButton("About GUtil", &_dialog);
            m_buttonList.append(aboutGUtil);
            aboutGUtil->setMinimumWidth(PUSH_BUTTON_WIDTH);
            connect(aboutGUtil, SIGNAL(clicked()),
                    this, SLOT(ShowAboutGUtil()));
        }
        QPushButton *btnLicense( 0 );
        if(show_license_button)
        {
            btnLicense = new QPushButton("Show License", &_dialog);
            m_buttonList.append(btnLicense);
            btnLicense->setMinimumWidth(PUSH_BUTTON_WIDTH);
            connect(btnLicense, SIGNAL(clicked()), this, SLOT(ShowLicense()));
        }
        QPushButton *ok( new QPushButton("Ok", &_dialog) );
        m_buttonList.append(ok);

        hbl->addWidget(aboutQt);
        hbl->addStretch(1);
        if(aboutGUtil)
        {
            hbl->addWidget(aboutGUtil);
            hbl->addStretch(1);
        }
        if(btnLicense)
        {
            hbl->addWidget(btnLicense);
            hbl->addStretch(1);
        }
        hbl->addWidget(ok);
        m_buttonWidget->setLayout(hbl);

        connect(aboutQt, SIGNAL(clicked()), this, SLOT(ShowAboutQt()));
        connect(ok, SIGNAL(clicked()), &_dialog, SLOT(accept()));

        vbl->addWidget(m_buttonWidget);

        ok->setFocus();
    }
    _dialog.setLayout(top_level_layout);

    _text.setReadOnly(true);
}

void About::SetImage(const QString &filename)
{
    if(filename.isNull())
    {
        m_imageFrame->hide();
        _dialog.setWindowIcon(QIcon());
    }
    else
    {
        m_imageFrame->setFixedSize(TITLE_HEIGHT, TITLE_HEIGHT);
        m_imageFrame->setStyleSheet(QString("image: url(%1);").arg(filename));
        m_imageFrame->show();
        _dialog.setWindowIcon(QIcon(filename));
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

void About::ShowAbout()
{
    _dialog.exec();
}


END_NAMESPACE_GUTIL2;


#endif // GUI_FUNCTIONALITY
