#ifndef FILE_MANAGER_TEST_H
#define FILE_MANAGER_TEST_H

#include <QObject>

class File_Manager;

class file_manager_test : public QObject
{
    Q_OBJECT
public:
    explicit file_manager_test(QObject *parent = 0);

signals:

public slots:

private slots:
    void simple_startup_test();
    void test_binary_dat();
    void test_reset();
    void test_large_files();

private:
    File_Manager *fm;

};

#endif // FILE_MANAGER_TEST_H
