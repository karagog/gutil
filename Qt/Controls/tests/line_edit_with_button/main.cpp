
#include <QApplication>
#include "test_form.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    test_form *tf = new test_form();

    tf->show();

    return app.exec();
}
