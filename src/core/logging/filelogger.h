/*Copyright 2010-2013 George Karagoulis

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

#include "gutil_file.h"
#include "gutil_abstractlogger.h"

NAMESPACE_GUTIL;


/** A logger that outputs to a. */
class FileLogger :
    public AbstractLogger
{
public:
    FileLogger(const char *filename)
        :AbstractLogger(new File(filename))
    {
        File *f(static_cast<File *>(io_device()));
        f->SetBufferedWrites(false);    // A log file must not delay writing
        f->Open(File::OpenAppend);
    }

    /** Erases the contents of the logfile. */
    virtual void Clear(){
        File *f( static_cast<File *>(io_device()) );
        f->Close();
        f->Open(File::OpenReadWriteTruncate);
    }
};


END_NAMESPACE_GUTIL;

#endif // GUTIL_FILELOGGER_H
