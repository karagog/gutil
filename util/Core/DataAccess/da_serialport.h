#include <string>

#ifdef _WIN32
	#include <windows.h>
#endif
using namespace std;

//This is the header for cross-platform Windows&Linux serial port functionality

namespace GUtil
{
    namespace Core
    {
        namespace DataAccess
        {
            class cp_serial_t{
                    public:
                            cp_serial_t(){}                                 //Default constructor does nothing; must configure!
                            ~cp_serial_t(){}
                            void Close();
                            int configure(string COMPORT);                                //Open the port, Setup the BAUD rate and such
                            int write_bytes(string bytes,int num);              //Returns 0 on successful write
                            int read_bytes(string & ret,int num);             //Returns 0 on successful read
                    private:
                    #ifdef linux
                            int handle;
                    #else
                            HANDLE handle;
                    #endif
            };
        }
    }
}
