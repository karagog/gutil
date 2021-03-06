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

#ifndef __APPLE__

#include <gutil/serialport.h>

#ifdef __unix__
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <cstring>
#endif

NAMESPACE_GUTIL;

SerialPort::SerialPort()
    :m_handle(0)
{}

SerialPort::~SerialPort()
{
    if(IsOpen()) Close();
}

void SerialPort::Initialize(const char *comport)
{
    // Function to configure port settings and open new com port
#ifdef __unix__

    m_handle = open(comport, O_RDWR | O_NOCTTY);
    if(m_handle == -1){
        throw Exception<>("Could not open com port");
    }

    struct termios options;
    tcgetattr(m_handle,&options);

    //Set the BAUD rate
    cfsetispeed(&options,B115200);
    cfsetospeed(&options,B115200);

    //disable hardware flow control
    options.c_cflag &= ~CRTSCTS;
    //options.c_cflag |= CNEW_RTSCTS;    /* Also called CRTSCTS */

    //Use Raw input
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //Use Raw Output
    options.c_oflag &= ~OPOST;

    options.c_cflag &= ~PARENB;       //No parity
    options.c_cflag &= ~CSTOPB;       //One stop bit
    options.c_cflag &= ~CSIZE;        //Clears the data size settings
    options.c_cflag |= CS8;           //Sets the data size to 8 bits

    options.c_iflag &= ~(INPCK | ISTRIP);   //turn off parity checking
    options.c_iflag |= (IGNPAR);

    //The minimum number of characters to read
    options.c_cc[VMIN] = 1;
    //Time to wait for minimum bits in tenths of a second (obsolete)
    options.c_cc[VTIME]= 0;

    //CLOCAL means don’t allow control of the port to be changed
    //CREAD says to enable the receiver
    options.c_cflag |= (CLOCAL | CREAD);

    if(tcsetattr(m_handle,TCSAFLUSH,&options) != 0){
        throw Exception<>("Could not set port options");
    }

    // Flush everything that's waiting on the port
    tcflush(m_handle, TCIOFLUSH);

    // Or it's windows
#else
    COMMTIMEOUTS timeouts={0};
    DCB dcbSerialParams = {0};

    // Open windows com port
    wchar_t comport_wchar[10];
    if(0 == MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
                                comport, -1,
                                comport_wchar, 10))
    {
        throw Exception<>("Invalid COM port");
    }

    m_handle = CreateFile(comport_wchar,
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          0,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          0);

    if(m_handle == INVALID_HANDLE_VALUE){
        throw Exception<>("Unable to open COM port");
    }

    //cout<<"Successfully opened "<<comport.c_str()<<endl;

    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
    if (!GetCommState(m_handle, &dcbSerialParams)) {
        CloseHandle(m_handle); // Give up
        throw Exception<>("Cannot get the port state");
    }

    // Next set the com port parameters
    dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if(!SetCommState(m_handle, &dcbSerialParams)) {
        CloseHandle(m_handle); // Give up
        throw Exception<>("Cannot set the port parameters");
    }

    // Set up timeouts
    timeouts.ReadIntervalTimeout=50;
    timeouts.ReadTotalTimeoutConstant=1000;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 1000;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if(!SetCommTimeouts(m_handle, &timeouts)){
        CloseHandle(m_handle); // Give up
        throw Exception<>("Cannot set the communication timeouts");
    }

    FlushFileBuffers(m_handle);             //make sure there's nothing left over in the buffer
#endif
}

void SerialPort::Close()
{
    if(IsOpen())
    {
#ifdef __unix__
        close(m_handle);
#else
        FlushFileBuffers(m_handle);             //clean up after yourself for the next guy to use the port
        CloseHandle(m_handle);
#endif
        m_handle = 0;
    }
}

GUINT32 SerialPort::ReadBytes(GBYTE *buf, GUINT32 buf_len, GUINT32 to_read)
{
#ifdef _WIN32
    DWORD bytesread = 0;
#else
    GUINT32 bytesread = 0;
#endif

    char tempchar;
    for(GUINT32 i = 0; i < to_read && i < buf_len; i++)
    {
#ifdef __unix__
        GUINT32 tmp_bytes_read = read(m_handle,&tempchar,1);
#else
        DWORD tmp_bytes_read = 0;
        ReadFile(m_handle, &tempchar, 1, &tmp_bytes_read, NULL);
#endif
        memcpy(buf, &tempchar, 1);

        buf += 1;
        bytesread += tmp_bytes_read;
    }

    return bytesread;
}

GUINT32 SerialPort::WriteBytes(const GBYTE *bytes, GUINT32 len)
{
#ifdef _WIN32
    DWORD byteswritten = 0;
#else
    GUINT32 byteswritten = 0;
#endif

    // Write the data one byte at a time
    /** \todo Improve the efficiency by writing blocks at a time.  In the past
        I had problems making that work, but maybe now I can do it.
    */
    for(GUINT32 i = 0; i < len; i++)
    {
#ifdef __unix__
        GUINT32 tmp_bytes = write(m_handle, bytes++, 1);
#else
        DWORD tmp_bytes = 0;
        WriteFile(m_handle, bytes++, 1, &tmp_bytes, NULL);
#endif

        byteswritten += tmp_bytes;
    }

    return byteswritten;
}


END_NAMESPACE_GUTIL;

#endif // __APPLE__
