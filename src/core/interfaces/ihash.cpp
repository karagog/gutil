/*Copyright 2014-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "ihash.h"
#include "iio.h"
#include <gutil/globals.h>
using namespace std;

NAMESPACE_GUTIL;


void IHash::AddDataFromDevice(IInput *input, GUINT32 chunk_size, function<bool(int)> progress_cb)
{
    GUINT32 len = input ? input->BytesAvailable() : 0;
    GUINT32 to_read = chunk_size == 0 ? len : Min(chunk_size, len);
    GUINT32 read = 0;
    const GUINT32 buf_sz = to_read;

    std::unique_ptr<byte[]> buf( new byte[buf_sz] );
    progress_cb(0);
    finally([=]{ progress_cb(100); });
    while(read < len)
    {
        if(to_read != input->ReadBytes(buf.get(), buf_sz, to_read))
            throw Exception<>("Error reading from device");

        AddData(buf.get(), to_read);
        read += to_read;

        if((read + to_read) > len)
            to_read = len - read;

        if(progress_cb(((float)read / len) * 100))
            throw CancelledOperationException<>();
    }
}


END_NAMESPACE_GUTIL;
