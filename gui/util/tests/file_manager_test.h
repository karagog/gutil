#ifndef FILE_MANAGER_TEST_H
#define FILE_MANAGER_TEST_H

#include <QObject>

namespace GUtil
{
    namespace QtUtil
    {
        class File_Manager;
    }
}

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
    void test_second_object();
    void test_large_files();
    void test_idList();

private:
    GUtil::QtUtil::File_Manager *fm;

};

#endif // FILE_MANAGER_TEST_H
