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

#ifndef GUTIL_FILELOGGER_H
#define GUTIL_FILELOGGER_H

#include "DataAccess/file.h"
#include "abstractlogger.h"

NAMESPACE_GUTIL1(Logging);


/** A logger that outputs to a. */
class FileLogger :
    public AbstractLogger
{
public:
    inline FileLogger(const char *filename)
        :AbstractLogger(new DataAccess::File(filename))
    {
        static_cast<DataAccess::File *>(io_device())
                ->Open(DataAccess::File::OpenAppend);
    }

    /** Erases the contents of the logfile. */
    inline void Clear(){
        DataAccess::File *f( static_cast<DataAccess::File *>(io_device()) );
        f->Close();
        f->Open(DataAccess::File::OpenWrite);
    }
};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_FILELOGGER_H
