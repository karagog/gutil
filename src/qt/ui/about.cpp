/*Copyright 2010-2015 George Karagoulis

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


#include <gutil/about.h>
#include <gutil/macros.h>
#include <gutil/application.h>
#include <gutil/licensewindow.h>
#include <gutil/aboutgutil.h>
#include <gutil/pluginutils.h>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QDir>
#include <QtPlugin>
#include <QResource>
#include <QMessageBox>

NAMESPACE_GUTIL1(Qt);

#define TITLE_HEIGHT 40
#define IMAGE_HEIGHT 60

#define PUSH_BUTTON_WIDTH 100

#define GUTIL_ABOUT_PLUGIN_NAME    "GUtilAboutPlugin"


static QPluginLoader s_pl;

/** A reference to the about plugin instance.  This is lazy-loaded when the user
 *   opens the first about window.
 */
static IAboutGUtil *si_ag( 0 );


void AboutLogic::ShowAboutQt()
{
    if(qApp) qApp->aboutQt();
}

void AboutLogic::ShowAboutGUtil(QWidget *parent)
{
    // Make sure the gutil about plugin is loaded
    _load_about_gutil_plugin();

    if(si_ag){
        si_ag->ShowAboutGUtil(parent);
    }
    else{
        QMessageBox::critical(0, "ERROR", "GUtil is really great, but I can't tell you about it because the necessary plugin won't load!", QMessageBox::Ok);
    }
}

QString AboutLogic::_load_about_gutil_plugin()
{
    QString error_msg;
    if(NULL == si_ag)
        si_ag = PluginUtils::LoadPlugin<IAboutGUtil>(s_pl, GUTIL_ABOUT_PLUGIN_NAME);
    return error_msg;
}

void AboutLogic::ShowLicense(QWidget *parent)
{
    LicenseWindow( get_license_text(), parent ).exec();
}

void AboutLogic::ShowNotice(QWidget *parent)
{
    LicenseWindow lw( get_notice_text(), parent );
    lw.setWindowTitle(tr("Notice"));
    lw.exec();
}

QString AboutLogic::get_license_text()
{
    QString ret;

    // Make sure the gutil about plugin is loaded
    QString err( _load_about_gutil_plugin() );

    // If the gutil about plugin has been successfully loaded
    if(si_ag)
    {
        // fetch the license info from the plugin's resources
        QResource r(":GUtil/About/license.txt");
        if(r.isValid()){
            ret = r.isCompressed() ? qUncompress(r.data(), r.size()) :
                                     QByteArray((const char *)r.data(), r.size());
        }
    }
    else
        ret = QString("(Unable to load resource: %1)").arg(err);

    return ret;
}

QString AboutLogic::get_notice_text()
{
    QString ret;
    QString err( _load_about_gutil_plugin() );

    if(si_ag)
    {
        QResource r(":GUtil/About/notice.txt");
        if(r.isValid()){
            ret = r.isCompressed() ? qUncompress(r.data(), r.size()) :
                                     QByteArray((const char *)r.data(), r.size());
        }
    }
    else
        ret = QString("(Unable to load resource: %1)").arg(err);
    return ret;
}



About::About(QWidget *parent,
             bool show_about_gutil_button,
             bool show_license_button,
             bool show_notice_button)
    :AboutLogic(parent),
      _dialog(parent),
      m_imageFrame(new QWidget(&_dialog)),
      m_buttonWidget(new QWidget(&_dialog))
{
    // Prepare the dialog layout
    _dialog.resize(400, 300);

    _header.setAlignment(::Qt::AlignHCenter);
    _buildinfo.setAlignment(::Qt::AlignHCenter);
    {
        QFont f;
        f.setBold(true);
        f.setPixelSize(20);
        _header.setFont(f);
        _header.setFixedHeight(TITLE_HEIGHT);
    }

    QVBoxLayout *top_level_layout(new QVBoxLayout);
    {
        QHBoxLayout *hbl(new QHBoxLayout);
        top_level_layout->addLayout(hbl);
        m_imageFrame->hide();
        hbl->addWidget(m_imageFrame);
        //hbl->setAlignment(m_imageFrame, ::Qt::AlignTop);

        QVBoxLayout *vbl( new QVBoxLayout );
        hbl->addLayout(vbl);
        vbl->addWidget(&_header);
        vbl->addWidget(&_buildinfo);
    }
    top_level_layout->addWidget(&_text);
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
            aboutGUtil = new QPushButton(tr("About GUtil"), &_dialog);
            m_buttonList.append(aboutGUtil);
            aboutGUtil->setMinimumWidth(PUSH_BUTTON_WIDTH);
            connect(aboutGUtil, SIGNAL(clicked()),
                    this, SLOT(_show_about_gutil()));
        }
        QPushButton *btnLicense( 0 );
        if(show_license_button)
        {
            btnLicense = new QPushButton(tr("License"), &_dialog);
            m_buttonList.append(btnLicense);
            btnLicense->setMinimumWidth(PUSH_BUTTON_WIDTH);
            connect(btnLicense, SIGNAL(clicked()), this, SLOT(_show_license()));
        }
        QPushButton *btnNotice( 0 );
        if(show_notice_button)
        {
            btnNotice = new QPushButton(tr("Notice"), &_dialog);
            m_buttonList.append(btnNotice);
            btnNotice->setMinimumWidth(PUSH_BUTTON_WIDTH);
            connect(btnNotice, SIGNAL(clicked()), this, SLOT(_show_notice()));
        }
        QPushButton *ok( new QPushButton(tr("Ok"), &_dialog) );
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
        if(btnNotice)
        {
            hbl->addWidget(btnNotice);
            hbl->addStretch(1);
        }
        hbl->addWidget(ok);
        m_buttonWidget->setLayout(hbl);

        connect(aboutQt, SIGNAL(clicked()), this, SLOT(ShowAboutQt()));
        connect(ok, SIGNAL(clicked()), &_dialog, SLOT(accept()));

        top_level_layout->addWidget(m_buttonWidget);

        ok->setFocus();
    }
    _dialog.setLayout(top_level_layout);

    _text.setReadOnly(true);
}

void About::_show_about_gutil()
{
    ShowAboutGUtil(_dialog.parentWidget());
}

void About::_show_license()
{
    ShowLicense(_dialog.parentWidget());
}

void About::_show_notice()
{
    ShowNotice(_dialog.parentWidget());
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
        m_imageFrame->setFixedSize(IMAGE_HEIGHT, IMAGE_HEIGHT);
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
    _dialog.show();
}


END_NAMESPACE_GUTIL1;


#endif // GUI_FUNCTIONALITY
