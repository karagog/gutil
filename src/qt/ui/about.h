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

#ifndef GUTIL_ABOUT_H
#define GUTIL_ABOUT_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include <gutil/exception.h>
#include <QDialog>
#include <QLabel>
#include <QTextEdit>

namespace GUtil{ namespace Qt{

class IAboutGUtil;


/** The business logic of the about window. */
class AboutLogic :
        public QObject
{
    Q_OBJECT
public slots:

    /** Tells this object to show its about content. */
    virtual void ShowAbout() = 0;

    /** Shows the application's license info.  You can override get_license_text()
        to customize your own text.
    */
    void ShowLicense(QWidget *parent = 0);

    /** Shows GUtil's about window. */
    static void ShowAboutGUtil(QWidget *parent = 0);

    /** Shows Qt's about window. */
    static void ShowAboutQt();


public:

    /** Constructor an AboutLogic instance. */
    explicit AboutLogic(QObject *parent = 0);

    /** Virtual destructor for dynamic type casting. */
    virtual ~AboutLogic();


protected:

    /** Returns the license text for the program.

        By default it returns the Apache License v2.0, because we assume anyone
        using this GUtil library is writing open-source applications.

        You can override the virtual function to customize your own license text if you need to.
    */
    virtual QString get_license_text();


private:

    /** Attempts to load the about GUtil plugin so you can use the About plugin and associated resources.
     *
     *  This does nothing if the plugin has already been loaded.
    */
    static QString _load_about_gutil_plugin();

};


/** A generic about window that you can use to quickly set your own. */
class About :
        public AboutLogic
{
    Q_OBJECT
public:

    /** Creates a new About instance.

        \param parent The widget which you would like to set as the About dialog's parent
        \param show_about_gutil_button Controls whether to show the button for "AboutGUtil"
        in the normal window.
        \param show_license_button Controls whether to show a license info button.  The implementation
        of the button click calls the virtual function get_license_text() as a content sourse for the
        license window.
    */
    explicit About(QWidget *parent = 0, bool show_about_gutil_button = true, bool show_license_button = false);

    /** Returns the title of the about window. */
    QString GetTitle() const{ return _header.text(); }

    /** Returns the build info string. */
    QString GetBuildInfo() const{ return _buildinfo.text(); }

    /** Returns the about window's text attribute as plaintext */
    QString GetAboutText() const{ return _text.toPlainText(); }


    /** Sets the window title for the about dialog. */
    void SetWindowTitle(const QString &t){ _dialog.setWindowTitle(t); }

    /** Sets an image to display in the window
        \param path_to_resource Can be a resource or a file path
    */
    void SetImage(const QString &path_to_resource);


    /** Adds your own custom pushbutton to the interface.
        This extends the window's functionality, because the button
        can do literally anything.
    */
    void AddPushButton(QPushButton *);


public slots:

    /** Shows the about window.  Overridden from AboutLogic. */
    virtual void ShowAbout();


protected:

    // Derived classes get control over the members of the about window

    /** The dialog window instance. */
    QDialog _dialog;

    /** Controls the header of the about window */
    QLabel _header;

    /** Gives information about the version and/or date */
    QLabel _buildinfo;

    /** Controls the text content of the window */
    QTextEdit _text;


private:

    QWidget *m_imageFrame;
    QWidget *m_buttonWidget;
    QWidgetList m_buttonList;

};


}}

#endif // GUI_FUNCTIONALITY

#endif // GUTIL_ABOUT_H
