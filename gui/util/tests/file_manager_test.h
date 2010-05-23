#ifndef FILE_MANAGER_TEST_H
#define FILE_MANAGER_TEST_H

#include <QObject>

class file_manager_test : public QObject
{
    Q_OBJECT
public:
    explicit file_manager_test(QObject *parent = 0);

signals:

public slots:

private slots:
    void simple_startup_test();

};

#endif // FILE_MANAGER_TEST_H
