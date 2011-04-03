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

#ifndef GLOBALMEMORYMANAGER_H
#define GLOBALMEMORYMANAGER_H

namespace GUtil{ namespace Utils{


// A class to encapsulate global memory.  You must derive from this class, and
//  declare global variables specific to your application
class GlobalMemoryManager
{
public:

    // Subclasses should not forget to declare their destructors virtual, in case
    //  they are subclassed
    virtual ~GlobalMemoryManager(){}


    // For keeping track of the global memory instance.  You can only set it once.
    static GlobalMemoryManager *GlobalInstance();
    static void SetGlobalInstance(GlobalMemoryManager *mm);


protected:

    // There is no public constructor, because this class is designed to be subclassed
    inline GlobalMemoryManager(){}


private:

    static GlobalMemoryManager *m_GlobalInstance;

};


}}

#endif // GLOBALMEMORYMANAGER_H
