#ifndef TEST_FORM_H
#define TEST_FORM_H

#include <QWidget>

namespace Ui {
    class test_form;
}

class test_form : public QWidget {
    Q_OBJECT
public:
    test_form(QWidget *parent = 0);
    ~test_form();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::test_form *ui;

private slots:
    void process_click();

    void progress_bar_clicked();
};

#endif // TEST_FORM_H
