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

#include <QHash>
#include <QReadWriteLock>

namespace GUtil{ namespace Utils{


// A class to encapsulate global memory.  You must derive from this class, and
//  declare global variables specific to your application
class GlobalMemoryManager
{
public:

    // Subclasses should not forget to declare their destructors virtual, in case
    //  they are subclassed
    virtual ~GlobalMemoryManager(){}

    // Use this macro when you subclass to create a globals class.  It will define the
    //  static getters and setters for the global instance.
#define DEFINE_GLOBAL_INSTANCE( NAME, CLASS ) \
    static inline CLASS *NAME##Instance(){ return (CLASS *)get_global_instance(#NAME); } \
    static inline void Set##NAME##Instance(CLASS *mm){ set_global_instance(#NAME, mm); } \
    enum{}


protected:

    // There is no public constructor, because this class is designed to be subclassed
    inline GlobalMemoryManager(){}

    // These functions are referenced in the public macros above, which you should use
    //  in your subclass to define the
    static void* get_global_instance(const QString &);
    static void  set_global_instance(const QString &, void *);


private:

    static QHash<QString, void*> m_GlobalInstances;
    static QReadWriteLock m_GlobalInstancesLock;

};


}}

#endif // GLOBALMEMORYMANAGER_H
