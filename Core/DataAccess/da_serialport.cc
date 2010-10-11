#include <string>
#include <iostream>
#include <stdio.h>
#include "da_serialport.h"

#ifdef linux
        #include <termios.h>
        #include <fcntl.h>
        #include <errno.h>
#endif

using namespace std;
using namespace GUtil::Core::DataAccess;

int cp_serial_t::configure(string COMPORT){
	// Function to configure port settings and open new com port
        //Returns -1 if error, 0 on success
#ifdef linux

        handle = open(COMPORT.c_str(),O_RDWR | O_NOCTTY);
        if(handle == -1){
                cout<<"ERROR: Could not open com port: "<<COMPORT<<endl;
				// Try the next com port:
                //COMPORT[COMPORT.size()-1]++;
                //handle = open(COMPORT.c_str(),O_RDWR | O_NOCTTY);
                return(-1);
        }
        cout<<"Opened port: "<<COMPORT<<endl;

        struct termios options;
        tcgetattr(handle,&options);

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

        if(tcsetattr(handle,TCSAFLUSH,&options) != 0){
                cout<<"ERROR: Could not set port options"<<endl;
                return(-1);
        }

		// Flush everything that's waiting on the port
		tcflush(handle, TCIOFLUSH);

// Or it's windows
#else
        COMMTIMEOUTS timeouts={0};
		DCB dcbSerialParams = {0};

        // Open windows com port
        //int count = 9;
        //do{
                //COMPORT[COMPORT.size()-1]++;            //Increment the number at the end to try another port
        handle = CreateFile((wchar_t*)COMPORT.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0);
        //}while((handle == INVALID_HANDLE_VALUE) && ((--count) > 0));
        if(handle == INVALID_HANDLE_VALUE){
			cout<<"Unable to open COM port: "<<COMPORT.c_str()<<endl;
                return(-1);
        }

    cout<<"Successfully opened "<<COMPORT.c_str()<<endl;

        dcbSerialParams.DCBlength=sizeof(dcbSerialParams);
        if (!GetCommState(handle, &dcbSerialParams)) {
            cout<<"Cannot get the port state."<<endl;
            CloseHandle(handle); // Give up
            return(-1);
        }

        // Next set the com port parameters
	dcbSerialParams.BaudRate = CBR_115200;
    dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if(!SetCommState(handle, &dcbSerialParams)) {
        cout<<"Cannot set the port parameters..."<<endl;
        CloseHandle(handle); // Give up
        return(-1);
    }

        // Set up timeouts
    timeouts.ReadIntervalTimeout=50;
    timeouts.ReadTotalTimeoutConstant=1000;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 1000;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if(!SetCommTimeouts(handle, &timeouts)){
        printf("cannot set the timeouts.\n");
        CloseHandle(handle); // Give up
                return(-1);
    }

    FlushFileBuffers(handle);             //make sure there's nothing left over in the buffer
#endif
        return 0;
}

void cp_serial_t::Close(){
#ifdef linux
        close(handle);
#else
    FlushFileBuffers(handle);             //clean up after yourself for the next guy to use the port
    CloseHandle(handle);
#endif
}

int cp_serial_t::read_bytes(string & ret,int num){
        //Returns 0 if successful
                //Sets 'ret' to a string made of the bytes read
#ifdef _WIN32
	DWORD dwBytesRead = 0;
#else
	int bytesread = 0;
#endif
		char tempchar = 0x00;
		ret = "";
		for(int i = 0; i < num; i++){
#ifdef linux
			bytesread = read(handle,&tempchar,1);
#else
			dwBytesRead = 0;
			ReadFile(handle, &tempchar, 1, &dwBytesRead, NULL);
#endif
			ret.resize(ret.size() + 1);
			ret[ret.size() - 1] = tempchar;
			tempchar = 0x00;
		}
        return(0);
}

int cp_serial_t::write_bytes(string bytes, int num){
#ifdef _WIN32
	DWORD dwBytesWritten = 0;
#else
	int byteswritten = 0;
#endif

	for(int i = 0; i < num; i++){
#ifdef linux
		byteswritten = write(handle,&bytes[i],1);
#else
		WriteFile(handle, &bytes[i], 1, &dwBytesWritten, NULL);
		dwBytesWritten = 0;
#endif
	}
	return 0;
}

