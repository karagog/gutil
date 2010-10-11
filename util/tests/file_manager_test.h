#ifndef FILE_MANAGER_TEST_H
#define FILE_MANAGER_TEST_H

#include <QObject>

namespace GQtUtil
{
    namespace DataAccess
    {
        class DA_BinaryDataStore;
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
    void test_file_queuing();
    void test_hasFile();
    void test_remove();

private:
    GQtUtil::DataAccess::DA_BinaryDataStore *fm;

};

#endif // FILE_MANAGER_TEST_H
