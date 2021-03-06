/*Copyright 2010-2015 George Karagoulis

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

#include <gutil/outputdevicelogger.h>
#include <gutil/file.h>

NAMESPACE_GUTIL;


/** A logger that outputs to a file.
    
    \note This uses calls to the C library fwrite(), which in some C library versions
    is thread safe, and in others is not. Therefore if you need to guarantee thread safety,
    protect this with a QueuedLogger.
*/
class FileLogger : public OutputDeviceLogger
{
    File *m_file;   // Gets deleted by the output device logger
public:
    /** Creates a FileLogger that logs to the given file. */
    FileLogger(const char *filename)
        :OutputDeviceLogger([&]{
            m_file = new File(filename);
            try{ m_file->Open(File::OpenAppend); }
            catch(...) { delete m_file; throw; }
            return m_file;
        }())
    {}

    /** Erases the contents of the logfile. */
    virtual void Clear(){
        m_file->Close();
        m_file->Open(File::OpenReadWriteTruncate);
    }
};


END_NAMESPACE_GUTIL;

#endif // GUTIL_FILELOGGER_H
