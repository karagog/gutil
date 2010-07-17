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

#ifndef FILETRANSPORT_H
#define FILETRANSPORT_H

#include "ITransportMechanism.h"

class QtLockedFile;

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            namespace Transports
            {
                // A simple mechanism for exchanging data with a file
                class FileTransport : public ITransportMechanism
                {
                public:
                    FileTransport(const QString &);

                    virtual void exportData(const QByteArray &);
                    virtual QByteArray importData();

                    QString fileName() const;

                private:
                    QtLockedFile *_lf;
                };
            }
        }
    }
}

#endif // FILETRANSPORT_H
