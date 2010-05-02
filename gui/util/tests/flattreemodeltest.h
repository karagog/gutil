#ifndef FLATTREEMODELTEST_H
#define FLATTREEMODELTEST_H

#include <QWidget>

namespace Ui {
    class flatTreeModelTest;
}

class flatTreeModelTest : public QWidget
{
    Q_OBJECT

public:
    explicit flatTreeModelTest(QWidget *parent = 0);
    ~flatTreeModelTest();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::flatTreeModelTest *ui;
};

#endif // FLATTREEMODELTEST_H
