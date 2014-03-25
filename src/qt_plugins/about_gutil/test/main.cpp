#include <QtGui/QApplication>

#include "gutil_application.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GUtil::QT::Application::AboutGUtil();

    //return a.exec();
    return 0;
}
