/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <QString>
#include <QMutex>
#include <QThread>
#include <QSqlDatabase>
#include <QObject>

// Use this class to manage binary data.  This is useful for storing and accessing
//  lots of files without consuming lots of memory

namespace GUtil
{
    namespace QtUtil
    {
        // This class will read/write the data to disk for us in a separate thread
        class File_Manager_worker : public QThread
        {
            Q_OBJECT
            friend class File_Manager;
        public:

            File_Manager_worker(const QString&,
                                QList<QString> &, QMutex &,
                                QList<int> &, QMutex &,
                                bool is_secondary,
                                QObject *parent = 0);
            ~File_Manager_worker();

            int addFile(const QString &);
            QString getFile(int) const;

        protected:
            void run();

            QList<int> idList();

            // Clear all files
            void reset();

        private:
            QMutex *add_mutex, *rem_mutex;
            QList<QString> *add_q;
            QList<int> *rem_q;

            QString my_id;
            QString file_location;
            int max_file_id;

            void get_database(QSqlDatabase &) const;
            static QString get_file_loc(const QString &id);
        };



        // This is the main class you interface with
        class File_Manager : public QObject
        {
            Q_OBJECT
        public:

            // If 'is_secondary' is true, then it won't automatically reset the file when
            //  it initializes itself.  Use this to preserve existing data
            File_Manager(const QString &unique_id, bool is_secondary = false);
            ~File_Manager();

            // Use these as an interface to access files (accepts and returns binary data strings)
            int addFile(const QString &data);
            QString getFile(int id) const;

            // Pushes the file changes from memory to the disk
            void pushToDisk();

            void reset();

            QList<int> idList();

        private slots:
            void worker_finished();

        private:
            QMutex add_q_mutex;
            QMutex rem_q_mutex;
            QList<QString> add_q;
            QList<int> rem_q;

            File_Manager_worker *worker;
            bool worker_running;
        };
    }
}

#endif // FILE_MANAGER_H
