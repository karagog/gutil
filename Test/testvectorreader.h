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

#ifndef GUTIL_TESTVECTORREADER_H
#define GUTIL_TESTVECTORREADER_H

#include "gutil_macros.h"
#include <QFile>

NAMESPACE_GUTIL1(Test);


/** A helper class that parses a test vector file, and gives you a simple api to
    read it line by line.  It will automatically strip the comments, so you only
    see the lines that are uncommented.
*/
class TestVectorReader
{
public:

    TestVectorReader(const QString &filename)
        :f(filename)
    {
        if(!f.exists())
            THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                       QString("File does not exist: %1").arg(filename).toUtf8().constData());
    }

    ~TestVectorReader(){
        if(f.isOpen())
            f.close();
    }

    bool ReadNextLine(QByteArray &ba){
        if(!f.isOpen())
        {
            if(!f.open(QFile::ReadOnly))
                THROW_NEW_GUTIL_EXCEPTION(Exception);
        }

        int cnt(0);
        bool ret(false);
        do
        {
            ba = f.readLine().trimmed();
            int tmpind = ba.indexOf('#');
            if(tmpind != -1)
            {
                ba.chop(ba.length() - tmpind);
                ba = ba.trimmed();
            }

            if(ba.length() > 0)
            {
                ret = true;
                break;
            }
        }while(++cnt < 1000 && !f.atEnd());

        return ret;
    }


private:
    QFile f;
};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_TESTVECTORREADER_H
