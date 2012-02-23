/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef DATATRANSFERUTILS_H
#define DATATRANSFERUTILS_H

#include "Core/macros.h"


class QString;
class QByteArray;

NAMESPACE_GUTIL1(Utils);
class PubSubSystem;
END_NAMESPACE_GUTIL1;


NAMESPACE_GUTIL2(QT, Utils);


/** A static class that declares functions which may help in
    transferring data from one place to another.
*/
class DataTransferUtils
{
public:

    /** Writes some data to a file in blocks of size 4096.
        It can work with the PubSubSystem class to report progress of the write.
        \todo Make the block size configurable
    */
    static void WriteDataToFileInChunks(const QString &filename,
                                        const QByteArray &data,
                                        GUtil::Utils::PubSubSystem *pss = 0,
                                        int progress_id = -1,
                                        bool *cancel_flag = 0);

    static void CopyFileInChunks(const QString &source,
                                 const QString &dest,
                                 GUtil::Utils::PubSubSystem *pss = 0,
                                 int progress_id = -1,
                                 bool *cancel_flag = 0);

    static QByteArray ReadDataFromFileInChunks(const QString &filename,
                                               GUtil::Utils::PubSubSystem *pss = 0,
                                               int progress_id = -1,
                                               bool *cancel_flag = 0);
};


END_NAMESPACE_GUTIL2;

#endif // DATATRANSFERUTILS_H
