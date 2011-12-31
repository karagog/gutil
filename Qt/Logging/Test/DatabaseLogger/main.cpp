/*Copyright 2011 George Karagoulis
  
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0
    
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <QtGui/QApplication>
#include "Logging/databaselogger.h"
#include "DataModels/databaselogmodel.h"
#include <QSqlDatabase>
#include <QTableView>
USING_NAMESPACE_GUTIL2(QT, Logging);
USING_NAMESPACE_GUTIL2(QT, DataModels);

#define TEST_CONN_STR   "test_connection"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QSqlDatabase db( QSqlDatabase::addDatabase("QSQLITE", TEST_CONN_STR));
    db.setDatabaseName("test_database.sqlite");

    DatabaseLogger dlog(TEST_CONN_STR);
    dlog.InitializeDatabase();

    dlog.Log("Hello World!", "First Message", DatabaseLogger::Warning);
    dlog.Log("Hello Sir Charles!", "Second Message", DatabaseLogger::Info);
    dlog.Log("Oh Dear!!!", "Error Message", DatabaseLogger::Error);
    
    QTableView *tbl(new QTableView);

    DatabaseLogModel *dblm( new DatabaseLogModel(dlog, tbl) );
    dblm->select();
    tbl->setModel(dblm);

    tbl->show();
    return a.exec();
}

