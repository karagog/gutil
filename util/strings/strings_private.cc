#include <string>
using namespace std;

bool translate_path(string &path)
{
    // Replace backslashes to forward slashes for compatibility

    unsigned int ind = path.find_first_of("\\");
    if(ind != string::npos)
    {
        // If there is a mix of forward slashes and backwards, I don't know how
        //  to handle it, so return false
        if(path.find_first_of("/") != string::npos)
            return false;

        do
        {
            path.replace(ind, 1, "/");
            ind = path.find_first_of("\\");
        }while(ind != string::npos);
    }

    // Strip the trailing slash so we get the right path name
    while(string::npos != (ind = path.find_last_of("/"))
        && ind == path.length() - 1)
        {
        path.resize(path.length() - 1);
    }

    return true;
}
