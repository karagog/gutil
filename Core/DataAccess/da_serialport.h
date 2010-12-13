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

#ifndef CP_SERIAL_T_H
#define CP_SERIAL_T_H

#include "gutil_macros.h"
#include <string>

#ifdef _WIN32
	#include <windows.h>
#endif

//This is the header for cross-platform Windows&Linux serial port functionality

GUTIL_BEGIN_CORE_NAMESPACE( DataAccess );


class cp_serial_t
{
public:

    cp_serial_t(){}                                 //Default constructor does nothing; must configure!
    virtual ~cp_serial_t(){}

    void Close();
    int configure(std::string COMPORT);                                //Open the port, Setup the BAUD rate and such
    int write_bytes(std::string bytes, int num);              //Returns 0 on successful write
    int read_bytes(std::string & ret, int num);             //Returns 0 on successful read


private:
#ifdef linux
    int handle;
#else
    HANDLE handle;
#endif
};


GUTIL_END_CORE_NAMESPACE

#endif  // CP_SERIAL_T_H
