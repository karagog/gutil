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

#include "gutil_consoleiodevice.h"
USING_NAMESPACE_GUTIL2(QT, DataAccess);

#define CNT_MAX 8

int main(int, char **)
{
    {
        ConsoleIODevice first_device;
        first_device.WriteLine("Testing two console io devices.  Press Enter to continue...");
        if(!first_device.WaitForReadyRead(3 * 1000))
            first_device.WriteLine("Fine if you won't participate I will go on without you!");
    }

    ConsoleIODevice cio(true);
    GUINT64 line_id = 0;

    cio.WriteLine(QString("Counting to %1").arg(CNT_MAX));

    for(int i = 0; i < CNT_MAX; ++i)
        line_id = cio.WriteLine(QString("Counting:  %1").arg(i + 1));

    //cio.WaitForLineWritten(2);
    cio.WaitForLineWritten(line_id);

    // Even though we write this at the end, the console io device is
    //  writing on a background thread, so this may appear before the
    //  counting text if we don't wait.  Very cool!
    printf("Finished Counting\n");

    return 0;
}
